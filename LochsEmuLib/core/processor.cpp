#include "stdafx.h"
#include "processor.h"
#include "pemodule.h"
#include "emulator.h"
#include "thread.h"
#include "stack.h"
#include "pluginmgr.h"
#include "coprocessor.h"
#include "winapi.h"
#include "process.h"

BEGIN_NAMESPACE_LOCHSEMU()

Processor::Processor(int id, Thread *thread) : IntID(id)
{
    Assert(thread);
    m_thread = thread;
}

Processor::~Processor()
{
    Mem = NULL;
    m_emulator = NULL;
}


LxResult Processor::Initialize()
{
    Mem = m_thread->Mem();
    m_process = m_thread->Proc();
    m_emulator = m_process->Emu();
    m_plugins = m_thread->Plugins();
    Reset();

    ESP = m_thread->GetStack()->Top();
    V( m_fpu.Initialize() );
    SIMD.Initialize();
    Exception.Initialize(this);
    RET_SUCCESS();
}

void Processor::Reset()
{
    EAX = ECX = EDX = EBX = ESP = EBP = ESI = EDI = ZERO = 0;
    CS = DS = SS = ES = FS = GS = 0;
    CF = PF = AF = ZF = SF = TF = IF = DF = OF = IOPL = NT = RF
        = VM = AC = VIF = VIP = ID = 0;
    EIP = 0;
    GP_Regs8[0] = &AL;
    GP_Regs8[1] = &CL;
    GP_Regs8[2] = &DL;
    GP_Regs8[3] = &BL;
    GP_Regs8[4] = &AH;
    GP_Regs8[5] = &CH;
    GP_Regs8[6] = &DH;
    GP_Regs8[7] = &BH;
    GP_Regs8[8] = &(GP_Regs[8].H8);
    m_terminated = true;
    ZeroMemory(m_callbackTable, sizeof(u32) * LX_CALLBACKS);
    m_inst = NULL;
    m_fpu.Reset();
    m_currSection = NULL;
    m_lastEip = 0;
    ClearExecFlags();
}

LxResult Processor::Step()
{
    // look up the inst decode cache
    m_inst = m_instCache.Lookup(EIP);

    if (NULL == m_inst) {
        // fetch at current EIP
        pbyte codePtr = GetCodePtr();
        m_inst = new Instruction();
        LxDecode(codePtr, m_inst, EIP);
        m_instCache.Insert(EIP, m_inst);
    }

    m_currSection = Mem->GetSection(EIP);

    /*
     * let plugins do their work
     */
    m_plugins->OnProcessorPreExecute(this, m_inst);

    V( Execute(m_inst) );

    m_plugins->OnProcessorPostExecute(this, m_inst);

    Assert(EIP == TERMINATE_EIP || Mem->Contains(EIP));

    // clear execution flag
    ClearExecFlags();

    RET_SUCCESS();
}

LxResult Processor::Run(u32 entry)
{
    EIP = entry; 

    LxInfo("Running Thread[%x] at EIP[0x%08x] ESP[0x%08X]\n", m_thread->ExtID, EIP, ESP);

    m_terminated = false;
    while (true) {
        LxResult lr = Step();
        if (LX_FAILED(lr)) { RET_FAIL(lr); }
        if (m_terminated) { break; }
        if (EIP == TERMINATE_EIP) {
            m_thread->ExitCode = EAX;
            break;
        }
    }

    LxDebug("Thread [%x] terminated\n", m_thread->ExtID);
    RET_SUCCESS();
}

LxResult Processor::RunCallback(uint id)
{
    //LxDebug("Running callback #%d\n", id);
    u32 entry = GetCallbackEntry(id);

    V( RunConditional(entry) );
    RET_SUCCESS();
}


LxResult Processor::RunConditional( u32 entry )
{
    //LxDebug("Running conditional(entry = %x)\n", entry);
    EIP = entry;

    m_terminated = false;
    while (true) {
        SetExecFlag(LX_EXEC_CALLBACK);
        LxResult lr = Step();
        if (LX_FAILED(lr)) { RET_FAIL(lr); }
        if (m_terminated || EIP == TERMINATE_EIP) break;
    }
    RET_SUCCESS();
}



LxResult Processor::Execute( const Instruction *inst )
{
    m_lastEip = EIP;
    EIP += inst->Length;

    InstHandler h;

    if (INST_ONEBYTE(inst->Main.Inst.Opcode)) {
        h = InstTableOneByte[inst->Main.Inst.Opcode];
    } else if (INST_TWOBYTE(inst->Main.Inst.Opcode)) {
        h = InstTableTwoBytes[inst->Main.Inst.Opcode & 0xff];
    } else {
        LxFatal("Unsupported instruction: %s\n", inst->Main.CompleteInstr);
    }

    // check for AddressSizeOverride prefix(67h)
    if (inst->Main.Prefix.AddressSize) {
        LxWarning("AddressSize prefix(67h) found at [%08x] %s\n",
            EIP - inst->Length, inst->Main.CompleteInstr);
    }

    // quick hack for 'rep ret' instructions; thanks to damn AMD
    bool isRet = inst->Main.Inst.BranchType == RetType;

    if (inst->Main.Prefix.LockPrefix) {
        Mem->Lock();
    }

    const u32 opcode = inst->Main.Inst.Opcode;
    // Thanks to shitty MOVQ_F30F7E
    if (inst->Main.Prefix.RepPrefix && !isRet && opcode != 0x0f7e) {
        while (ECX != 0) {
            (this->*h)(inst);
            ECX--;
            if (ECX == 0) break;

            bool isRepe = opcode == 0xa6 || opcode == 0xa7 || opcode == 0xae || opcode == 0xaf;
            if (isRepe && ZF == 0) break;
        }
        SetExecFlag(LX_EXEC_PREFIX_REP);
    } else if (inst->Main.Prefix.RepnePrefix) {
        while (ECX != 0) {
            (this->*h)(inst);
            ECX--;
            if (ECX == 0) break;
            if (ZF == 1) break;
        }
        SetExecFlag(LX_EXEC_PREFIX_REPNE);
    } else {
        (this->*h)(inst);
    }

    if (inst->Main.Prefix.LockPrefix) {
        Mem->Unlock();
    }

    RET_SUCCESS();
}

void Processor::PushContext()
{
    m_stack.push(EAX);
    m_stack.push(ECX);
    m_stack.push(EDX);
    m_stack.push(EBX);
    m_stack.push(ESP);
    m_stack.push(EBP);
    m_stack.push(ESI);
    m_stack.push(EDI);

    u32 eflags = GetEflags();
    m_stack.push(eflags);

//     m_stack.push(CF);
//     m_stack.push(PF);
//     m_stack.push(AF);
//     m_stack.push(ZF);
//     m_stack.push(SF);
//     m_stack.push(OF);

    //LxWarning("Only part of CPU context is perserved in CPU::PushContext\n");
}

void Processor::PopContext()
{
//     OF = m_stack.top(); m_stack.pop();
//     SF = m_stack.top(); m_stack.pop();
//     ZF = m_stack.top(); m_stack.pop();
//     AF = m_stack.top(); m_stack.pop();
//     PF = m_stack.top(); m_stack.pop();
//     CF = m_stack.top(); m_stack.pop();
    u32 eflags = m_stack.top(); m_stack.pop();
    SetEflags(eflags);

    EDI = m_stack.top(); m_stack.pop();
    ESI = m_stack.top(); m_stack.pop();
    EBP = m_stack.top(); m_stack.pop();
    ESP = m_stack.top(); m_stack.pop();
    EBX = m_stack.top(); m_stack.pop();
    EDX = m_stack.top(); m_stack.pop();
    ECX = m_stack.top(); m_stack.pop();
    EAX = m_stack.top(); m_stack.pop();
}


void Processor::Push( u32 nBytes, cpbyte content )
{
    Assert(Mem->GetSection(ESP) != NULL);
    Assert(Mem->GetSection(ESP - nBytes) == Mem->GetSection(ESP));

    ESP -= nBytes;
    memcpy(Mem->GetRawData(ESP), content, nBytes);
}

void Processor::Pop( u32 nBytes, pbyte content )
{
    Assert(Mem->GetSection(ESP) != NULL);
    Assert(Mem->GetSection(ESP + nBytes) == Mem->GetSection(ESP));

    memcpy(content, Mem->GetRawData(ESP), nBytes);
    ESP += nBytes;
}


/************************************************************************/
/* TODO : segment register, exception handling                          */
/************************************************************************/
INLINE u8 Processor::MemRead8( u32 address, RegSeg seg ) const
{
    u8 val = INIT_8;
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Read8(address, &val); 
    m_plugins->OnProcessorMemRead(this, address, 1, (cpbyte) &val);
    return val;
}

INLINE u16 Processor::MemRead16( u32 address, RegSeg seg ) const
{
    u16 val = INIT_16;
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Read16(address, &val);
    m_plugins->OnProcessorMemRead(this, address, 2, (cpbyte) &val);
    return val;
}

INLINE u32 Processor::MemRead32( u32 address, RegSeg seg ) const
{
    u32 val = INIT_32;
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Read32(address, &val);
    m_plugins->OnProcessorMemRead(this, address, 4, (cpbyte) &val);
    return val;
}

INLINE u64 Processor::MemRead64( u32 address, RegSeg seg ) const
{
    u64 val = INIT_64;
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Read64(address, &val);
    m_plugins->OnProcessorMemRead(this, address, 8, (cpbyte) &val);
    return val;
}

INLINE u128 Processor::MemRead128( u32 address, RegSeg seg ) const
{
    u128 val;
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Read128(address, &val);
    m_plugins->OnProcessorMemRead(this, address, 16, (cpbyte) &val);
    return val;
}

INLINE void Processor::MemWrite8( u32 address, u8 val, RegSeg seg )
{
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Write8(address, val);
    m_plugins->OnProcessorMemWrite(this, address, 1, (cpbyte) &val);
}

INLINE void Processor::MemWrite16( u32 address, u16 val, RegSeg seg )
{
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Write16(address, val);
    m_plugins->OnProcessorMemWrite(this, address, 2, (cpbyte) &val);
}

INLINE void Processor::MemWrite32( u32 address, u32 val, RegSeg seg )
{
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Write32(address, val);
    m_plugins->OnProcessorMemWrite(this, address, 4, (cpbyte) &val);
}

INLINE void Processor::MemWrite64( u32 address, u64 val, RegSeg seg )
{
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Write64(address, val);
    m_plugins->OnProcessorMemWrite(this, address, 8, (cpbyte) &val);
}

INLINE void Processor::MemWrite128( u32 address, const u128 &val, RegSeg seg )
{
    if (seg == LX_REG_FS) { address = GetFSOffset(address); }
    Mem->Write128(address, val);
    m_plugins->OnProcessorMemWrite(this, address, 16, (cpbyte) &val);
}

u32 Processor::GetFSOffset(u32 addr) const {
    return m_thread->GetTEBAddress() + addr;
}

u32 Processor::GetEflags() const
{
    u32 r = 0;
    r |= CF;
    r |= (PF << 2);
    r |= (AF << 4);
    r |= (ZF << 6);
    r |= (SF << 7);
    r |= (TF << 8);
    r |= (IF << 9);
    r |= (DF << 10);
    r |= (OF << 11);
    r |= (IOPL << 12);
    r |= (NT << 14);
    r |= (RF << 16);
    r |= (VM << 17);
    r |= (AC << 18);
    r |= (VIF << 19);
    r |= (VIP << 20);
    r |= (ID << 21);
    return r;
}

void Processor::SetEflags( u32 eflags )
{
    CF =     eflags & 1;
    PF =    (eflags >> 2) & 1;
    AF =    (eflags >> 4) & 1;
    ZF =    (eflags >> 6) & 1;
    SF =    (eflags >> 7) & 1;
    TF =    (eflags >> 8) & 1;
    IF =    (eflags >> 9) & 1;
    DF =    (eflags >> 10) & 1;
    OF =    (eflags >> 11) & 1;
    IOPL =  (eflags >> 12) & 3;
    NT =    (eflags >> 14) & 1;
    RF =    (eflags >> 16) & 1;
    VM =    (eflags >> 17) & 1;
    AC =    (eflags >> 18) & 1;
    VIF =   (eflags >> 19) & 1;
    VIP =   (eflags >> 20) & 1;
    ID =    (eflags >> 21) & 1;
}

void Processor::ToContext( CONTEXT *context )
{
    Assert(context);

    context->SegGs      = this->GS;
    context->SegFs      = this->FS;
    context->SegEs      = this->ES;
    context->SegDs      = this->DS;
    context->SegCs      = this->CS;
    context->SegSs      = this->SS;

    context->Edi        = this->EDI;
    context->Esi        = this->ESI;
    context->Ebx        = this->EBX;
    context->Edx        = this->EDX;
    context->Ecx        = this->ECX;
    context->Eax        = this->EAX;
    context->Ebp        = this->EBP;
    context->Esp        = this->ESP;

    context->Eip        = this->EIP;
    context->EFlags     = GetEflags();

    LxWarning("TODO: Processor::ToContext()  FPU & SIMD context\n");
}



void Processor::FromContext( const CONTEXT *context )
{
    Assert(context);

    this->GS            = (u16) context->SegGs;
    this->FS            = (u16) context->SegFs;
    this->ES            = (u16) context->SegEs;
    this->DS            = (u16) context->SegDs;
    this->CS            = (u16) context->SegCs;
    this->SS            = (u16) context->SegSs;

    this->EDI           = context->Edi;
    this->ESI           = context->Esi;
    this->EBX           = context->Ebx;
    this->EDX           = context->Edx;
    this->ECX           = context->Ecx;
    this->EAX           = context->Eax;
    this->EBP           = context->Ebp;
    this->ESP           = context->Esp;

    this->EIP           = context->Eip;
    SetEflags(context->EFlags);

    LxWarning("TODO: Processor::FromContext()  FPU & SIMD context\n");
}

uint Processor::GetCurrentModule( void ) const
{
    if (CurrentSection() == NULL) {
        LxError("Current section is NULL\n");
        return LX_UNKNOWN_MODULE;
    }
    return CurrentSection()->Module();
}

uint Processor::GetModule( u32 eip ) const
{
    return Mem->GetSection(eip)->Module();
}

u8 Processor::ReadOperand8( const Instruction *inst, const ARGTYPE &oper, u32 *offset ) const
{
    if (IsRegArg(oper)) {
        return GP_Reg8(REG_NUM(oper.ArgType), oper.ArgPosition);
    } else if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        u32 o = Offset32(oper);
        if (offset) *offset = o;
        return MemRead8(o, seg);
    } else if (IsConstantArg(oper)) {
        return (u8) inst->Main.Inst.Immediat;
    } else {
        LxFatal("ReadOperand8() called with no operand\n");
    }
    return 0;
}

u16 Processor::ReadOperand16( const Instruction *inst, const ARGTYPE &oper, u32 *offset ) const
{
    if (IsRegArg(oper)) {
        return GP_Reg16(REG_NUM(oper.ArgType));
    } else if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        u32 o = Offset32(oper);
        if (offset) *offset = o;
        return MemRead16(o, seg);
    } else if (IsConstantArg(oper)) {
        return (u16) inst->Main.Inst.Immediat;
    } else {
        LxFatal("ReadOperand16() called with no operand\n");
    }
    return 0;
}

u32 Processor::ReadOperand32( const Instruction *inst, const ARGTYPE &oper, u32 *offset ) const
{
    if (IsRegArg(oper)) {
        return GP_Reg32(REG_NUM(oper.ArgType));
    } else if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        u32 o = Offset32(oper);
        if (offset) *offset = o;
        return MemRead32(o, seg);
    } else if (IsConstantArg(oper)) {
        return (u32) inst->Main.Inst.Immediat;
    } else {
        LxFatal("ReadOperand32() called with no operand\n");
    }
    return 0;
}

u64 Processor::ReadOperand64(const Instruction *inst, const ARGTYPE &oper, u32 *offset) const
{
    if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        u32 o = Offset32(oper);
        if (offset) *offset = o;
        return MemRead64(o, seg);
    } else if (IsRegArg(oper) && (REG_TYPE(oper.ArgType) & MMX_REG)) {
        return SIMD.MMReg(REG_NUM(oper.ArgType));
    } else {
        LxFatal("Processor::ReadOperand64() called with non-memory operand\n");
    }
    return 0;
}

u128 Processor::ReadOperand128(const Instruction *inst, const ARGTYPE &oper, u32 *offset) const
{
    if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        u32 o = Offset32(oper);
        if (offset) *offset = o;
        return MemRead128(o, seg);
    } else if (IsRegArg(oper) && (REG_TYPE(oper.ArgType) & SSE_REG)) {
        return SIMD.XMMReg(REG_NUM(oper.ArgType));
    } else {
        LxFatal("Processor::ReadOperand128() invalid operand\n");
    }
    return u128();
}

void Processor::WriteOperand8( const Instruction *inst, const ARGTYPE &oper, u32 offset, u8 val )
{
    if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        MemWrite8(offset, val, seg);
    } else if (IsRegArg(oper)) {
        GP_Reg8(REG_NUM(oper.ArgType), oper.ArgPosition) = val;
    } else if (IsConstantArg(oper)) {
        LxFatal("WriteOperand8() called with immediate operand\n");
    } else {
        LxFatal("WriteOperand8() called with no operand\n");
    }
}

void Processor::WriteOperand16( const Instruction *inst, const ARGTYPE &oper, u32 offset, u16 val )
{
    if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        MemWrite16(offset, val, seg);
    } else if (IsRegArg(oper)) {
        GP_Reg16(REG_NUM(oper.ArgType)) = val;
    } else if (IsConstantArg(oper)) {
        LxFatal("WriteOperand16() called with immediate operand\n");
    } else {
        LxFatal("WriteOperand16() called with no operand\n");
    }
}

void Processor::WriteOperand32( const Instruction *inst, const ARGTYPE &oper, u32 offset, u32 val )
{
    if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        MemWrite32(offset, val, seg);
    } else if (IsRegArg(oper)) {
        GP_Reg32(REG_NUM(oper.ArgType)) = val;
    } else if (IsConstantArg(oper)) {
        LxFatal("WriteOperand32() called with immediate operand\n");
    } else {
        LxFatal("WriteOperand32() called with no operand\n");
    }
}

void Processor::WriteOperand64(const Instruction *inst, const ARGTYPE &oper, u32 offset, const u64 &val)
{
    if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        MemWrite64(offset, val, seg);
    } else if (IsRegArg(oper) && (REG_TYPE(oper.ArgType) & MMX_REG)) {
        SIMD.MMReg(REG_NUM(oper.ArgType)) = val;
    } else {
        LxFatal("Processor::WriteOperand64() called with non-memory operand\n");
    }
}

void Processor::WriteOperand128(const Instruction *inst, const ARGTYPE &oper, u32 offset, const u128 &val)
{
    if (IsMemoryArg(oper)) {
        RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
        MemWrite128(offset, val, seg);
    } else if (IsRegArg(oper) && (REG_TYPE(oper.ArgType) & SSE_REG)){
        SIMD.XMMReg(REG_NUM(oper.ArgType)) = val;
    } else {
        LxFatal("Processor::WriteOperand128() invalid operand\n");
    }
}

bool Processor::IsJumpTaken( const Instruction *inst ) const
{
    typedef     bool (Processor::*Tester)(void) const;

    static Tester Testers[] = {
        /* 0 */ &Processor::IsJumpTaken_Jo,
        /* 1 */ &Processor::IsJumpTaken_Jno,
        /* 2 */ &Processor::IsJumpTaken_Jb,
        /* 3 */ &Processor::IsJumpTaken_Jnb,
        /* 4 */ &Processor::IsJumpTaken_Je,
        /* 5 */ &Processor::IsJumpTaken_Jne,
        /* 6 */ &Processor::IsJumpTaken_Jbe,
        /* 7 */ &Processor::IsJumpTaken_Ja,
        /* 8 */ &Processor::IsJumpTaken_Js,
        /* 9 */ &Processor::IsJumpTaken_Jns,
        /* a */ &Processor::IsJumpTaken_Jp,
        /* b */ &Processor::IsJumpTaken_Jnp,
        /* c */ &Processor::IsJumpTaken_Jl,
        /* d */ &Processor::IsJumpTaken_Jge,
        /* e */ &Processor::IsJumpTaken_Jle,
        /* f */ &Processor::IsJumpTaken_Jg,
    };

    Tester t = NULL;
    u32 opcode = inst->Main.Inst.Opcode;
    if (opcode >= 0x70 && opcode < 0x80) {
        t = Testers[opcode - 0x70];
    } else if (opcode >= 0x0f80 && opcode < 0x0f90) {
        t = Testers[opcode - 0x0f80];
    } else if (opcode >= 0x0f90 && opcode < 0x0fa0) {
        t = Testers[opcode - 0x0f90];
    } else if (opcode == 0xe3) {
        t = &Processor::IsJumpTaken_Jecxz;
    } else if (opcode == 0xe2) {
        t = &Processor::IsJumpTaken_Loop;
    } else if (opcode >= 0x0f40 && opcode < 0x0f50) {
        t = Testers[opcode - 0x0f40];       // CMOVcc
    } else {
        Assert(0);  // shouldn't be used for other instructions
    }
    Assert(t != NULL);
    return (this->*t)();
}

INLINE u32 Processor::GetCallbackEntry( uint callbackId ) const
{
    Assert(callbackId < LX_CALLBACKS);  
    u32 entry = m_callbackTable[callbackId]; 

    if (entry == 0 && callbackId == LX_CALLBACK_FLS) {
        // Fls related hack
        // When a thread exits, this FLS callback is invoked
        // However, its entry is registered in main thread's callback table
        // So retrieve it from main processor
        entry = m_emulator->GetProcessorMain()->GetCallbackEntry(callbackId);
    }

    return entry;
}

void Processor::Terminate( uint nCode )
{
    LxInfo("Processor [%x] terminating with exit code 0x%x\n", m_thread->ExtID, nCode);
    m_terminated = true; 
    m_thread->ExitCode = nCode;
}

u32 Processor::GetValidEip( void ) const
{
    if (HasExecFlag(LX_EXEC_WINAPI_CALL) || HasExecFlag(LX_EXEC_WINAPI_JMP) ||
        HasExecFlag(LX_EXEC_PREFIX_REP) || HasExecFlag(LX_EXEC_PREFIX_REPNE))
    {
        return GetPrevEip();
    } else {
        return EIP;
    }
}

END_NAMESPACE_LOCHSEMU()