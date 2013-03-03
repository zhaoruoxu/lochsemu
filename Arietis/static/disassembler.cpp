#include "stdafx.h"
#include "disassembler.h"
#include "engine.h"
#include "event.h"

#include "processor.h"
#include "instruction.h"
#include "memory.h"


InstSection::InstSection( InstMem *mem, InstPool &pool, u32 base, u32 size )
    : m_mem(mem), m_pool(pool), m_base(base), m_size(size), m_count(0)
{
    m_data      = new InstPtr[m_size];
    m_indices   = new u32[m_size];
    ZeroMemory(m_data, sizeof(InstPtr) * m_size);
    for (u32 i = 0; i < m_size; i++)
        m_indices[i] = -1;
}

InstSection::~InstSection()
{
    SAFE_DELETE_ARRAY(m_data);
    SAFE_DELETE_ARRAY(m_indices);
}

InstPtr InstSection::Alloc( u32 addr )
{
    SyncObjectLock lock(*this);

    AssertInRanage(addr);
    Assert(m_data[addr - m_base] == NULL);
    InstPtr pinst           = m_pool.Alloc();
    pinst->Eip              = addr;
    m_data[addr - m_base]   = pinst;
    m_count++;
    return pinst;
}

InstPtr * InstSection::Next( InstPtr *curr ) const
{
    InstPtr *p = curr;
    while (true) {
        p++;
        if (*p != NULL || p == End()) return p;
    }
}

InstPtr * InstSection::Begin() const
{
    InstPtr *p = m_data;
    while (*p == NULL && p != End()) 
        p++;
    return p;
}

void InstSection::UpdateIndices() const
{
    int idx = 0;
    for (u32 i = 0; i < m_size; i++)
        m_indices[i] = -1;
    for (u32 i = 0; i < m_size; i++) {
        if (m_data[i] != NULL) {
            m_data[i]->Index = idx;
            m_indices[idx++] = m_data[i]->Eip;
        }
    }
}

void InstSection::Lock() const
{
    m_mem->Lock();
}

void InstSection::Unlock() const
{
    m_mem->Unlock();
}

InstMem::InstMem() : m_pool(16384) //, m_mutex(false)
{
    ZeroMemory(m_pagetable, sizeof(m_pagetable));
}

InstMem::~InstMem()
{
    int ptr = 0;
    while (ptr < LX_PAGE_COUNT) {
        if (m_pagetable[ptr] == NULL) {
            ptr++;
            continue;
        }
        int s = m_pagetable[ptr]->GetSize() / LX_PAGE_SIZE;
        Assert(s > 0);
        SAFE_DELETE(m_pagetable[ptr]);
        ptr += s;
    }
}

InstSection * InstMem::CreateSection( u32 base, u32 size )
{
    InstSection *sec = GetSection(base);
    if (sec) {
        Assert(sec->GetBase() == base && sec->GetSize() == size);
        return sec;
    }
    return AddSection(base, size);
}

InstSection * InstMem::AddSection( u32 base, u32 size )
{
    InstSection *sec = new InstSection(this, m_pool, base, size);
    for (uint i = PAGE_NUM(base); i < PAGE_NUM(base + size); i++) {
        Assert(m_pagetable[i] == NULL);
        m_pagetable[i] = sec;
    }
    return sec;
}

InstPtr InstMem::GetInst( u32 addr ) const
{
    InstSection *sec = GetSection(addr);
    Assert(sec);
    return sec->GetInst(addr);
}

Disassembler::Disassembler()
{
    m_dataUpdateHandler = nullptr;
    m_lastSec           = NULL;
    m_currProcessor     = NULL;
}

Disassembler::~Disassembler()
{

}

void Disassembler::OnPreExecute( PreExecuteEvent &event )
{
    m_currProcessor = event.Cpu;

    u32 eip = m_currProcessor->EIP;
    Section *sec = m_currProcessor->Mem->GetSection(eip);
    
    bool update = false;
    InstSection *instSec = m_instMem.CreateSection(sec->Base(), sec->Size());

    {
        SyncObjectLock lock(m_instMem);

        if (!instSec->Contains(eip)) {
            LxDebug("Disassembling %08x...\n", eip);
            RecursiveDisassemble(m_currProcessor, eip, instSec, eip);
            LxDebug("Disassemble complete, count = %d\n", instSec->GetCount());
            //instSec->UpdateIndices();
            update = true;
        }
    }

    if (m_lastSec != sec || update) {
        //if (m_dataUpdateHandler) 
        instSec->UpdateIndices();
        m_dataUpdateHandler(instSec, m_currProcessor);
    }
    m_lastSec = sec;
}

void Disassembler::RecursiveDisassemble( const Processor *cpu, u32 eip, InstSection *sec, u32 entryEip )
{
    Assert(sec);
    Assert(entryEip != 0);

    while (true) {
        Assert(sec->IsInRange(eip));
        if (sec->Contains(eip)) return;     // already disassembled

        InstPtr inst = sec->Alloc(eip);
        LxDecode(cpu->Mem->GetRawData(eip), (Instruction *) inst, eip);
        AttachApiInfo(cpu, eip, sec, inst);

        u32 opcode = inst->Main.Inst.Opcode;
        if (opcode == 0xc3 || opcode == 0xcb || opcode == 0xc2 || opcode == 0xca) {
            // 'ret' is met
            inst->Entry = entryEip;
            return;
        }

        u32 addrValue = (u32) inst->Main.Inst.AddrValue;
        if (addrValue != 0) {
            if (IsConstantArg(inst->Main.Argument1)) {
                Section *s = cpu->Mem->GetSection(addrValue);
                if (s != NULL) {
                    u32 nextEntry = Instruction::IsCall(inst) ? addrValue : entryEip;
                    InstSection *jumpSec = m_instMem.CreateSection(s->Base(), s->Size());
                    RecursiveDisassemble(cpu, addrValue, jumpSec, nextEntry);
                }
            }
        }
        u32 nextEip = eip + inst->Length;
        InstSection *nextSec = m_instMem.GetSection(nextEip);
        if (sec != nextSec) return;
        eip = nextEip;
    }
}

void Disassembler::AttachApiInfo( const Processor *cpu, u32 eip, InstSection *sec, InstPtr inst )
{
    u32 target = 0;
    u32 opcode = inst->Main.Inst.Opcode;
    const char *mnemonic = inst->Main.Inst.Mnemonic;
    if (opcode == 0xff) {
        // CALL or JMP r/m32
        if (strstr(mnemonic, "jmp") == mnemonic || Instruction::IsCall(inst)) {
            if (IsMemoryArg(inst->Main.Argument1) &&
                inst->Main.Argument1.Memory.BaseRegister == 0 &&
                inst->Main.Argument1.Memory.IndexRegister == 0) {
                target = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
            }
        }
    } else if (opcode == 0xe8) {
        target = (u32) inst->Main.Inst.AddrValue;
        
        Section *sect = cpu->Mem->GetSection(target);
        if (sect) {
            InstSection *callSec = m_instMem.CreateSection(sect->Base(), sect->Size());
            RecursiveDisassemble(cpu, target, callSec, target);

            InstPtr instCalled = callSec->GetInst(target);
            Assert(instCalled != NULL);

            if (instCalled->Main.Inst.Opcode == 0xff &&
                (strstr(instCalled->Main.Inst.Mnemonic, "jmp") == instCalled->Main.Inst.Mnemonic)) 
            {
                target = cpu->ReadOperand32(instCalled, instCalled->Main.Argument1, NULL);
            }
        }
    } else if (*mnemonic == 'j') {
        target = (u32) inst->Main.Inst.AddrValue;
    }

    if (target) {
        inst->Target = target;
    }

    const ApiInfo *info = cpu->Proc()->GetApiInfoFromAddress(target);
    if (info) {
        strncpy(inst->DllName, info->ModuleName.c_str(), sizeof(inst->DllName));
        strncpy(inst->FuncName, info->FunctionName.c_str(), sizeof(inst->FuncName));
    }
}

void Disassembler::UpdateInstContext( InstContext *ctx, u32 eip ) const
{
    Assert(m_currProcessor);

    InstPtr inst    = m_instMem.GetInst(eip);
    ctx->inst       = inst;
}
