#pragma once

#ifndef __CORE_PROCESSOR_H__
#define __CORE_PROCESSOR_H__

#include "lochsemu.h"
#include "memory.h"
#include "instruction.h"
#include "callback.h"
#include "win32.h"
#include "debug.h"
#include "pluginmgr.h"
#include "hashtable.h"
#include "simd.h"
#include "exception.h"

BEGIN_NAMESPACE_LOCHSEMU()

    enum Reg8 {
        LX_REG_AL = 0,
        LX_REG_CL,
        LX_REG_DL,
        LX_REG_BL,
        LX_REG_AH,
        LX_REG_CH,
        LX_REG_DH,
        LX_REG_BH,
};

enum Reg16 {
    LX_REG_AX = 0,
    LX_REG_CX,
    LX_REG_DX,
    LX_REG_BX,
    LX_REG_SP,
    LX_REG_BP,
    LX_REG_SI,
    LX_REG_DI,
};

enum Reg32 {
    LX_REG_EAX = 0,
    LX_REG_ECX,
    LX_REG_EDX,
    LX_REG_EBX,
    LX_REG_ESP,
    LX_REG_EBP,
    LX_REG_ESI,
    LX_REG_EDI,
    LX_REG_NONE
};

enum RegSeg {
    LX_REG_ES = 0,
    LX_REG_CS,
    LX_REG_SS,
    LX_REG_DS,
    LX_REG_FS,
    LX_REG_GS
};

enum RegMM {
    LX_REG_MM0 = 0,
    LX_REG_MM1,
    LX_REG_MM2,
    LX_REG_MM3,
    LX_REG_MM4,
    LX_REG_MM5,
    LX_REG_MM6,
    LX_REG_MM7,
};

enum RegXMM {
    LX_REG_XMM0 = 0,
    LX_REG_XMM1,
    LX_REG_XMM2,
    LX_REG_XMM3,
    LX_REG_XMM4,
    LX_REG_XMM5,
    LX_REG_XMM6,
    LX_REG_XMM7,
};

#define MAX_INT8        127
#define MIN_INT8        -128
#define MAX_UINT8       255
#define MIN_UINT8       0
#define MAX_INT16       32767
#define MIN_INT16       -MAX_INT16 -1
#define MAX_UINT16      65535
#define MIN_UINT16      0
#define MAX_INT32       2147483647
#define MIN_INT32       -MAX_INT32 -1
#define MAX_UINT32	    4294967295U
#define MIN_UINT32      0

// Promote U 8/16/32 to I/U 16/32/64
#define PROMOTE_I16(x)        ((i16) (i8) (x))
#define PROMOTE_U16(x)        ((u16) (x))
#define PROMOTE_I32(x)        ((i32) (i16) (x))
#define PROMOTE_U32(x)        ((u32) (x))
#define PROMOTE_I64(x)        ((i64) (i32) (x))
#define PROMOTE_U64(x)        ((u64) (x))

// Most Significant Bit & Least Significant Bit
#define MSB8(x)     ((((u8) x) >> 7) & 1)
#define MSB16(x)    ((((u16) x) >> 15) & 1)
#define MSB32(x)    ((((u32) x) >> 31) & 1)
#define LSB(x)      ((x) & 0x1)

// Sign-Extend
#define SIGN_EXTEND(from_bits, to_bits, value)  ((u##to_bits)(i##to_bits)(i##from_bits) (value))

// Prefix operand size override
#define OPERAND_16BIT(x)        ((x) & PREFIX_OPERAND_SIZE_OVERRIDE)

// Used in callback function; when EIP==CALLBACK_RET_EIP, callback will return
#define TERMINATE_EIP        0x0


// Execution flags
#define LX_EXEC_WINAPI_CALL     0x1
#define LX_EXEC_WINAPI_JMP      0x2
#define LX_EXEC_PREFIX_REP      0x4
#define LX_EXEC_PREFIX_REPNE    0x8
#define LX_EXEC_CALLBACK        0x16
#define LX_EXEC_TERMINATE_EIP   0x32

class LX_API Processor {
    // Simulation for x86 CPU
public:
    Processor(Thread *thread);
    virtual ~Processor();

public:
    /************************************************************************/
    /* Members                                                              */
    /************************************************************************/
    Memory *Mem;

    // General-Purpose Registers
    union {
        union {
            u32 X32; u16 X16; struct { u8 L8; u8 H8; };
        } GP_Regs[9];   // [8] is fixed ZERO
        struct {
            union { u32 EAX; u16 AX; struct { u8 AL; u8 AH; }; };
            union { u32 ECX; u16 CX; struct { u8 CL; u8 CH; }; };
            union { u32 EDX; u16 DX; struct { u8 DL; u8 DH; }; };
            union { u32 EBX; u16 BX; struct { u8 BL; u8 BH; }; };
            union { u32 ESP; u16 SP; };
            union { u32 EBP; u16 BP; };
            union { u32 ESI; u16 SI; };
            union { u32 EDI; u16 DI; };
            u32 ZERO;   // DO NOT CHANGE THIS VALUE
        };
    };
    u8 *GP_Regs8[9];

    // Segment Registers
    union {
        u16 Seg_Regs[6];
        struct {
            u16 ES;
            u16 CS;
            u16 SS;
            u16 DS;
            u16 FS;
            u16 GS;
        };
    };

    // EFLAGS
    u32 CF;     //  0(Status): Carry Flag
    u32 PF;     //  2(Status): Parity Flag
    u32 AF;     //  4(Status): Auxiliary Carry Flag
    u32 ZF;     //  6(Status): Zero Flag
    u32 SF;     //  7(Status): Sign Flag
    u32 TF;     //  8(System): Trap Flag
    u32 IF;     //  9(System): Interrupt Enable Flag
    u32 DF;     // 10(Control): Direction Flag
    u32 OF;     // 11(Status): Overflow Flag
    u32 IOPL;   // 12,13(System): I/O Privilege Level
    u32 NT;     // 14(System): Nested Task
    u32 RF;     // 16(System): Resume Flag
    u32 VM;     // 17(System): Virtual-8086 Mode
    u32 AC;     // 18(System): Alignment Check
    u32 VIF;    // 19(System): Virtual Interrupt Flag
    u32 VIP;    // 20(System): Virtual Interrupt Pending
    u32 ID;     // 21(System): ID Flag

    // Instruction Pointer
    u32 EIP;

    // MMX & SSE
    X86SIMD             SIMD;

    // Exception handling
    ExceptionManager    Exception;

    /************************************************************************/
    /* Member Functions                                                     */
    /************************************************************************/
    Thread *        Thr                 (void) const { return m_thread; }
    Process *       Proc                (void) const { Assert(m_process); return m_process; }
    Emulator *      Emu                 (void) const { Assert(m_emulator); return m_emulator; }
    Coprocessor *   FPU                 (void) const { Assert(m_fpu); return m_fpu; }
    const Instruction * CurrentInst     (void) const { return m_inst; }
    Section *       CurrentSection      (void) const { return m_currSection; }

    /*
     * Get module number according to current EIP
     */
    uint            GetCurrentModule    (void) const;
    uint            GetModule           (u32 eip) const;

    u32             GetPrevEip          (void) const { return m_lastEip; }

    LxResult        Initialize          (void);
    LxResult        Run                 (u32 entry);
    LxResult        RunCallback         (uint id);
    LxResult        RunConditional      (u32 entry);
    LxResult        Step                (void);
    LxResult        Execute             (const Instruction *inst);
    void            Reset               (void);
    INLINE void     Terminate           (uint nCode);
    void            PushContext         (void);
    void            PopContext          (void);
    u32             GetEflags           (void) const;
    void            SetEflags           (u32 eflags);
    INLINE u32  &   Reg_32              (const ARGTYPE &oper);
    INLINE u16  &   Reg_16              (const ARGTYPE &oper);
    INLINE u8   &   Reg_8               (const ARGTYPE &oper);
    INLINE u32  &   GP_Reg32            (uint num);
    INLINE u16  &   GP_Reg16            (uint num);
    INLINE u8   &   GP_Reg8             (uint num, int pos);
    INLINE u32      Reg_32              (const ARGTYPE &oper) const;
    INLINE u16      Reg_16              (const ARGTYPE &oper) const;
    INLINE u8       Reg_8               (const ARGTYPE &oper) const;
    INLINE u32      GP_Reg32            (uint num) const;
    INLINE u16      GP_Reg16            (uint num) const;
    INLINE u8       GP_Reg8             (uint num, int pos) const;

    void            ClearExecFlags      (void) { m_execFlags = 0; }
    void            SetExecFlag         (u32 flag) { m_execFlags |= flag; }
    bool            HasExecFlag         (u32 flag) const { return (m_execFlags & flag) == flag; }

    /************************************************************************/
    /* Memory R/W Handlers                                                  */
    /************************************************************************/
    INLINE u8       MemRead8            (u32 address, RegSeg seg = LX_REG_DS) const;
    INLINE u16      MemRead16           (u32 address, RegSeg seg = LX_REG_DS) const;
    INLINE u32      MemRead32           (u32 address, RegSeg seg = LX_REG_DS) const;
    INLINE u64      MemRead64           (u32 address, RegSeg seg = LX_REG_DS) const;
    INLINE u128     MemRead128          (u32 address, RegSeg seg = LX_REG_DS) const;
    INLINE void     MemWrite8           (u32 address, u8 val, RegSeg seg = LX_REG_DS);
    INLINE void     MemWrite16          (u32 address, u16 val, RegSeg seg = LX_REG_DS);
    INLINE void     MemWrite32          (u32 address, u32 val, RegSeg seg = LX_REG_DS);
    INLINE void     MemWrite64          (u32 address, u64 val, RegSeg seg = LX_REG_DS);
    INLINE void     MemWrite128         (u32 address, const u128 &val, RegSeg seg = LX_REG_DS);

    /************************************************************************/
    /* Utilities                                                            */
    /************************************************************************/
    INLINE u32      GetStackParam32     (uint num) const;
    INLINE void *   GetStackParamPtr32  (uint num) const;
    pbyte           GetCodePtr          () const { Assert(Mem); return Mem->GetRawData(EIP); }
    pbyte           GetCodePtr          (u32 eip) const { return Mem->GetRawData(eip); }
    INLINE u32      GetCallbackEntry    (uint callbackId) const;
    INLINE void     SetCallbackEntry    (uint callbackId, u32 entry);
    INLINE u32      GetFSOffset         (u32 addr) const;

    // Push and Pop
    void            Push8               (u8 val)    { Push32(SIGN_EXTEND(8, 32, val)); }
    void            Push16              (u16 val)   { ESP -= 2; MemWrite16(ESP, val, LX_REG_SS); }
    void            Push32              (u32 val)   { ESP -= 4; MemWrite32(ESP, val, LX_REG_SS); }
    u8              Pop8                (void)      { return (u8) Pop32(); }
    u16             Pop16               (void)      { ESP += 2; return MemRead16(ESP-2, LX_REG_SS); }
    u32             Pop32               (void)      { ESP += 4; return MemRead32(ESP-4, LX_REG_SS); }

    // For internal use; no memory protection check
    void            Push                (u32 nBytes, cpbyte content);
    void            Pop                 (u32 nBytes, pbyte content);

    INLINE u32      Offset32            (const ARGTYPE &oper) const;

    // Set ZF according to result 'val'
    INLINE void     SetFlagZF8          (u8 val)    { ZF = (val == 0); }
    INLINE void     SetFlagZF16         (u16 val)   { ZF = (val == 0); }
    INLINE void     SetFlagZF32         (u32 val)   { ZF = (val == 0); }
    
    // Set PF according to 'val'
    INLINE void     SetFlagPF8          (u8 val);
    INLINE void     SetFlagPF16         (u16 val)   { SetFlagPF8((u8) val); }
    INLINE void     SetFlagPF32         (u32 val)   { SetFlagPF8((u8) val); }

    // Set SF according to 'val'
    INLINE void     SetFlagSF8          (u8 val);
    INLINE void     SetFlagSF16         (u16 val);
    INLINE void     SetFlagSF32         (u32 val);

    // Set CF according to 'val' and 'val64'
    INLINE void     SetFlagCF8          (u8 val,    u16 val16);
    INLINE void     SetFlagCF16         (u16 val,   u32 val32);
    INLINE void     SetFlagCF32         (u32 val,   const u64 &val64);

    // Set OF according to 'val' and 'val64'
    INLINE void     SetFlagOF8          (u8 val,    i16 val16);
    INLINE void     SetFlagOF16         (u16 val,   i32 val32);
    INLINE void     SetFlagOF32         (u32 val,   const i64 &val64);

    // Set arithmetic flags
    INLINE void     SetFlagsArith8      (u8 val,    u16 uv16,   i16 iv16);
    INLINE void     SetFlagsArith16     (u16 val,   u32 uv32,   i32 iv32);
    INLINE void     SetFlagsArith32     (u32 val,   const u64 &uv64,   const i64 &iv64);

    INLINE void     SetFlagsLogic8      (u8 val);
    INLINE void     SetFlagsLogic16     (u16 val);
    INLINE void     SetFlagsLogic32     (u32 val);

    INLINE void     SetFlagsShift8      (u8 val);
    INLINE void     SetFlagsShift16     (u16 val);
    INLINE void     SetFlagsShift32     (u32 val);

    // Read from mem/reg
    u8              ReadOperand8        (const Instruction *inst,   const ARGTYPE &oper,    u32 *offset) const;
    u16             ReadOperand16       (const Instruction *inst,   const ARGTYPE &oper,    u32 *offset) const;
    u32             ReadOperand32       (const Instruction *inst,   const ARGTYPE &oper,    u32 *offset) const;
    u64             ReadOperand64       (const Instruction *inst,   const ARGTYPE &oper,    u32 *offset) const;
    u128            ReadOperand128      (const Instruction *inst,   const ARGTYPE &oper,    u32 *offset) const;

    // Write to mem/reg
    void            WriteOperand8       (const Instruction *inst,   const ARGTYPE &oper,    u32 offset, u8 val);
    void            WriteOperand16      (const Instruction *inst,   const ARGTYPE &oper,    u32 offset, u16 val);
    void            WriteOperand32      (const Instruction *inst,   const ARGTYPE &oper,    u32 offset, u32 val);
    void            WriteOperand64      (const Instruction *inst,   const ARGTYPE &oper,    u32 offset, const u64 &val);
    void            WriteOperand128     (const Instruction *inst,   const ARGTYPE &oper,    u32 offset, const u128 &val);


    // Convert from/to CONTEXT structure
    void            ToContext           (CONTEXT *context);
    void            FromContext         (const CONTEXT *context);

    bool            IsJumpTaken_Jo() const    { return OF != 0; }
    bool            IsJumpTaken_Jno() const   { return OF == 0; }
    bool            IsJumpTaken_Jb() const    { return CF != 0; }
    bool            IsJumpTaken_Jnb() const   { return CF == 0; }
    bool            IsJumpTaken_Je() const    { return ZF != 0; }
    bool            IsJumpTaken_Jne() const   { return ZF == 0; }
    bool            IsJumpTaken_Jbe() const   { return CF != 0 || ZF != 0; }
    bool            IsJumpTaken_Ja() const    { return CF == 0 && ZF == 0; }
    bool            IsJumpTaken_Js() const    { return SF != 0; }
    bool            IsJumpTaken_Jns() const   { return SF == 0; }
    bool            IsJumpTaken_Jp() const    { return PF != 0; }
    bool            IsJumpTaken_Jnp() const   { return PF == 0; }
    bool            IsJumpTaken_Jl() const    { return SF != OF; }
    bool            IsJumpTaken_Jge() const   { return SF == OF; }
    bool            IsJumpTaken_Jle() const   { return ZF != 0 || SF != OF; }
    bool            IsJumpTaken_Jg() const    { return ZF == 0 && SF == OF; }
    bool            IsJumpTaken_Jecxz() const { return ECX == 0; }
    bool            IsJumpTaken_Loop() const  { return ECX != 0; }
    bool            IsJumpTaken     (const Instruction *inst) const;

protected:
    Thread *        m_thread;
    Process *       m_process;
    Emulator *      m_emulator;
    Coprocessor *   m_fpu;
    std::stack<u32> m_stack;
    Instruction *   m_inst;
    bool            m_terminated;
    u32             m_callbackTable[LX_CALLBACKS];
    Hashtable<Instruction>  m_instCache;
    u32             m_execFlags;    // Used to represent status after execution of each instruciton 
    Section *       m_currSection;
    u32             m_lastEip;
}; // class CPU


INLINE u32 Processor::Reg_32( const ARGTYPE &oper ) const
{
    switch (REG_TYPE(oper.ArgType)) {
        case REGISTER_TYPE | GENERAL_REG:
            {
                return GP_Reg32(REG_NUM(oper.ArgType));
            } break;
        default:
            NOT_IMPLEMENTED();
    }
    Assert(!"Invalid register type");
    return EAX;
}

INLINE u16 Processor::Reg_16( const ARGTYPE &oper ) const
{
    switch (REG_TYPE(oper.ArgType)) {
        case REGISTER_TYPE | GENERAL_REG:
            {
                return GP_Reg16(REG_NUM(oper.ArgType));
            } break;
        default:
            NOT_IMPLEMENTED();
    }
    Assert(!"Invalid register type");
    return AX;
}

INLINE u8 Processor::Reg_8( const ARGTYPE &oper ) const
{
    switch (REG_TYPE(oper.ArgType)) {
        case REGISTER_TYPE | GENERAL_REG:
            {
                return GP_Reg8(REG_NUM(oper.ArgType), oper.ArgPosition);
            } break;
        default:
            NOT_IMPLEMENTED();
    }
    Assert(!"Invalid register type");
    return AL;
}

INLINE u32 Processor::GP_Reg32( uint num ) const
{
    return GP_Regs[LxRegConvert(num)].X32;
}

INLINE u16 Processor::GP_Reg16( uint num ) const
{
    switch (num) {
        case REG0: { return AX; } break;
        case REG1: { return CX; } break;
        case REG2: { return DX; } break;
        case REG3: { return BX; } break;
        case REG4: { return SP; } break;
        case REG5: { return BP; } break;
        case REG6: { return SI; } break;
        case REG7: { return DI; } break;
        default:
            LxFatal("Invalid 16-bit GP register: %x\n", num);
    }
    Assert(!"should never reach here");
    return AX;
}

INLINE u8 Processor::GP_Reg8( uint num, int pos ) const
{
    switch (num) {
        case REG0: { return *( ((u8p) &EAX) + pos ); } break;
        case REG1: { return *( ((u8p) &ECX) + pos); } break;
        case REG2: { return *( ((u8p) &EDX) + pos ); } break;
        case REG3: { return *( ((u8p) &EBX) + pos ); } break;
        default:
            LxFatal("Invalid 8-bit GP register: %x\n", num);
    }
    Assert(!"should never reach here");
    return AL;
}

INLINE u32 & Processor::Reg_32( const ARGTYPE &oper )
{
    switch (REG_TYPE(oper.ArgType)) {
        case REGISTER_TYPE | GENERAL_REG:
            {
                return GP_Reg32(REG_NUM(oper.ArgType));
            } break;
        default:
            NOT_IMPLEMENTED();
    }
    Assert(!"Invalid register type");
    return EAX;
}

INLINE u16 & Processor::Reg_16( const ARGTYPE &oper )
{
    switch (REG_TYPE(oper.ArgType)) {
        case REGISTER_TYPE | GENERAL_REG:
            {
                return GP_Reg16(REG_NUM(oper.ArgType));
            } break;
        default:
            NOT_IMPLEMENTED();
    }
    Assert(!"Invalid register type");
    return AX;
}

INLINE u8 & Processor::Reg_8( const ARGTYPE &oper )
{
    switch (REG_TYPE(oper.ArgType)) {
        case REGISTER_TYPE | GENERAL_REG:
            {
                return GP_Reg8(REG_NUM(oper.ArgType), oper.ArgPosition);
            } break;
        default:
            NOT_IMPLEMENTED();
    }
    Assert(!"Invalid register type");
    return AL;
}


INLINE u32  & Processor::GP_Reg32( uint num )
{
    return GP_Regs[LxRegConvert(num)].X32;
}

INLINE u16  & Processor::GP_Reg16( uint num )
{
    switch (num) {
        case REG0: { return AX; } break;
        case REG1: { return CX; } break;
        case REG2: { return DX; } break;
        case REG3: { return BX; } break;
        case REG4: { return SP; } break;
        case REG5: { return BP; } break;
        case REG6: { return SI; } break;
        case REG7: { return DI; } break;
        default:
            LxFatal("Invalid 16-bit GP register: %x\n", num);
    }
    Assert(!"should never reach here");
    return AX;
}

INLINE u8   & Processor::GP_Reg8( uint num, int pos )
{
    switch (num) {
        case REG0: { return *( ((u8p) &EAX) + pos ); } break;
        case REG1: { return *( ((u8p) &ECX) + pos); } break;
        case REG2: { return *( ((u8p) &EDX) + pos ); } break;
        case REG3: { return *( ((u8p) &EBX) + pos ); } break;
        default:
            LxFatal("Invalid 8-bit GP register: %x\n", num);
    }
    Assert(!"should never reach here");
    return AL;
}


INLINE void Processor::SetCallbackEntry(uint callbackId, u32 entry) {
    Assert(callbackId < LX_CALLBACKS); m_callbackTable[callbackId] = entry;
}

INLINE u32 Processor::GetCallbackEntry(uint callbackId) const { 
    Assert(callbackId < LX_CALLBACKS);  return m_callbackTable[callbackId]; 
}

INLINE void Processor::Terminate(uint nCode) { 
    LxInfo("Process terminating with exit code 0x%x\n", nCode);
    m_terminated = true; 
}

INLINE u32 Processor::Offset32( const ARGTYPE &oper ) const
{
    u32 offset = (u32) oper.Memory.Displacement; //(oper.displacement & DisplacementMask[oper.dispbytes]);
    if (oper.Memory.BaseRegister) {
        offset += GP_Reg32(REG_NUM(oper.Memory.BaseRegister));
    }
    if (oper.Memory.IndexRegister) {
        u32 scale = oper.Memory.Scale;
        offset += GP_Reg32(REG_NUM(oper.Memory.IndexRegister)) * scale;
    }
    return offset;
}

INLINE void Processor::SetFlagPF8( u8 val )
{
    int num = 0;

    for (uint i = 0; i < 8; i++) {
        if (val & (1 << i)) ++num;
    }

    PF = ((num & 1) == 0);
}

INLINE void Processor::SetFlagSF8( u8 val )
{
    SF = (val & (1 << 7)) != 0;
}

INLINE void Processor::SetFlagSF16( u16 val )
{
    SF = (val & (1 << 15)) != 0;
}

INLINE void Processor::SetFlagSF32( u32 val )
{
    SF = (val & (1 << 31)) != 0;
}

INLINE void Processor::SetFlagCF8( u8 val, u16 val16 )
{
    CF = (val16 < MIN_UINT8 || val16 > MAX_UINT8 || val16 != (u16) val);
}

INLINE void Processor::SetFlagCF16( u16 val, u32 val32 )
{
    CF = (val32 < MIN_UINT16 || val32 > MAX_UINT16 || val32 != (u32) val);
}

INLINE void Processor::SetFlagCF32( u32 val, const u64 &val64 )
{
    CF = (val64 < MIN_UINT32 || val64 > MAX_UINT32 || val64 != (u64) val);
}

INLINE void Processor::SetFlagOF8( u8 val, i16 val16 )
{
    OF = (val16 < MIN_INT8 || val16 > MAX_INT8 || val16 != PROMOTE_I16(val));
}

INLINE void Processor::SetFlagOF16( u16 val, i32 val32 )
{
    OF = (val32 < MIN_INT16 || val32 > MAX_INT16 || val32 != PROMOTE_I32(val));
}

INLINE void Processor::SetFlagOF32( u32 val, const i64 &val64 )
{
    OF = (val64 < MIN_INT32 || val64 > MAX_INT32 || val64 != PROMOTE_I64(val));
}

INLINE void Processor::SetFlagsArith8( u8 val, u16 uv16, i16 iv16 )
{
    SetFlagCF8(val, uv16);
    SetFlagOF8(val, iv16);
    SetFlagZF8(val);
    SetFlagPF8(val);
    SetFlagSF8(val);
}

INLINE void Processor::SetFlagsArith16( u16 val, u32 uv32, i32 iv32 )
{
    SetFlagCF16(val, uv32);
    SetFlagOF16(val, iv32);
    SetFlagZF16(val);
    SetFlagPF16(val);
    SetFlagSF16(val);
}

INLINE void Processor::SetFlagsArith32( u32 val, const u64 &uv64, const i64 &iv64 )
{
    SetFlagCF32(val, uv64);
    SetFlagOF32(val, iv64);
    SetFlagZF32(val);
    SetFlagPF32(val);
    SetFlagSF32(val);
}

INLINE void Processor::SetFlagsLogic8( u8 val )
{
    SetFlagsShift8(val);
    OF = CF = 0;
}

INLINE void Processor::SetFlagsLogic16( u16 val )
{
    SetFlagsShift16(val);
    OF = CF = 0;
}

INLINE void Processor::SetFlagsLogic32( u32 val )
{
    SetFlagsShift32(val);
    OF = CF = 0;
}

INLINE void Processor::SetFlagsShift8( u8 val )
{
    SetFlagZF8(val);
    SetFlagPF8(val);
    SetFlagSF8(val);
}

INLINE void Processor::SetFlagsShift16( u16 val )
{
    SetFlagZF16(val);
    SetFlagPF16(val);
    SetFlagSF16(val);
}

INLINE void Processor::SetFlagsShift32( u32 val )
{
    SetFlagZF32(val);
    SetFlagPF32(val);
    SetFlagSF32(val);
}


INLINE u32 Processor::GetStackParam32( uint num ) const
{
    return MemRead32(ESP + (num+1) * 4, LX_REG_SS);
}

INLINE void * Processor::GetStackParamPtr32( uint num ) const
{
    u32 offset = GetStackParam32(num);
    return offset == 0 ? NULL : (void *) Mem->GetRawData(offset);
}

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_PROCESSOR_H__
