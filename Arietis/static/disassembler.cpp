#include "stdafx.h"
#include "disassembler.h"
#include "engine.h"

#include "processor.h"
#include "instruction.h"
#include "memory.h"


InstSection::InstSection( InstPool &pool, Mutex &mutex, u32 base, u32 size )
    : m_pool(pool), m_mutex(mutex), m_base(base), m_size(size), m_count(0)
{
    m_data      = new InstPtr[m_size];
    m_indices   = new u32[m_size];
    ZeroMemory(m_data, sizeof(InstPtr) * m_size);
    ZeroMemory(m_indices, sizeof(u32) * m_size);
//     for (u32 i = 0; i < m_size; i++)
//         m_indices[i] = -1;
}

InstSection::~InstSection()
{
    SAFE_DELETE_ARRAY(m_data);
    SAFE_DELETE_ARRAY(m_indices);
}


// void InstSection::AddInst( u32 addr, InstPtr pinst )
// {
//     AssertInRanage(addr);
//     if (m_data[addr - m_base] != NULL) {
//         LxWarning("Arietis.Disassembler: Adding an instruction to an exsiting one\n");
//     }
//     m_data[addr - m_base] = pinst;
// }

InstPtr InstSection::Alloc( u32 addr )
{
    Lock();
    AssertInRanage(addr);
    //if (m_data[addr - m_base] != NULL);
    Assert(m_data[addr - m_base] == NULL);
    InstPtr pinst           = m_pool.Alloc();
    pinst->Eip              = addr;
    m_data[addr - m_base]   = pinst;
    m_count++;
    Release();
    return pinst;
}

void InstSection::Lock() const
{
    m_mutex.Wait();
}

void InstSection::Release() const
{
    m_mutex.Release();
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
    for (u32 i = 0; i < m_size; i++) {
        if (m_data[i] != NULL) {
            m_data[i]->Index = idx;
            m_indices[idx++] = m_data[i]->Eip;
        }
    }
}

InstMem::InstMem() : m_pool(16384), m_mutex(false)
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
    InstSection *sec = new InstSection(m_pool, m_mutex, base, size);
    for (uint i = PAGE_NUM(base); i < PAGE_NUM(base + size); i++) {
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
    //m_ptrChangeHandler  = nullptr;
    m_dataUpdateHandler = nullptr;
    m_lastSec           = NULL;
    m_currProcessor     = NULL;
}

Disassembler::~Disassembler()
{

}

void Disassembler::OnPreExecute( const Processor *cpu, const Instruction *inst )
{
    m_currProcessor = cpu;

    u32 eip = cpu->EIP;
    Section *sec = cpu->Mem->GetSection(eip);
    
    m_instMem.Lock();

//     if (m_secMap.find(sec) == m_secMap.end()) {
//         m_secMap[sec] = InstDisasmMap();
//     }
    InstSection *instSec = m_instMem.CreateSection(sec->Base(), sec->Size());
    //if (NULL == instSec) {
    //    instSec = m_instMem.AddSection(sec->Base(), sec->Size());
    //}

    bool update = false;
    //const InstDisasmMap &instMap = m_secMap[sec];
    
    if (!instSec->Contains(eip)) {
        LxDebug("Disassembling %08x...\n", eip);
        RecursiveDisassemble(0, cpu, eip, instSec, eip);

//         int count = 0;
//         for (auto &iter : m_secMap) 
//             count += iter.second.size();
        LxDebug("Disassemble complete, count = %d\n", instSec->GetCount());
        instSec->UpdateIndices();
        update = true;
    }

    m_instMem.Release();

    if (m_lastSec != sec || update) {
        if (m_dataUpdateHandler) m_dataUpdateHandler(instSec);
    }
    m_lastSec = sec;
}

void Disassembler::RecursiveDisassemble( int depth, const Processor *cpu, u32 eip, InstSection *sec, u32 entryEip )
{
    Assert(sec);
    Assert(entryEip != 0);

    if (depth >= MaxRecursiveDepth) return;

    while (true) {
        //InstSection *instSec = m_instMem.GetSection(eip);
        Assert(sec->IsInRange(eip));
        if (sec->Contains(eip)) return;
        //if (instMap.find(eip) != instMap.end()) return; // already disassembled

        //InstPtr inst = new Instruction;
        InstPtr inst = sec->Alloc(eip);
        LxDecode(cpu->Mem->GetRawData(eip), (Instruction *) inst, eip);
        //instMap[eip] = Inst(inst, eip);

        AttachApiInfo(cpu, eip, sec, inst);
        //LxInfo("%08x  %s\n", eip, inst->Main.CompleteInstr);

        u32 opcode = inst->Main.Inst.Opcode;
        if (opcode == 0xc3 || opcode == 0xcb || opcode == 0xc2 || opcode == 0xca) {
            // 'ret' is met
            //instMap[eip].entry = entryEip;
            inst->Entry = entryEip;
            return;
        }

        u32 addrValue = (u32) inst->Main.Inst.AddrValue;
        //const char *mnemonics = inst->Main.Inst.Mnemonic;
        if (addrValue != 0) {
            if (OPERAND_TYPE(inst->Main.Argument1.ArgType) == CONSTANT_TYPE) {
                Section *s = cpu->Mem->GetSection(addrValue);
                if (s != NULL) {
                    u32 nextEntry = Instruction::IsCall(inst) ? addrValue : entryEip;
                    InstSection *jumpSec = m_instMem.CreateSection(s->Base(), s->Size());
                    RecursiveDisassemble(depth + 1, cpu, addrValue, jumpSec, nextEntry);
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
            if (OPERAND_TYPE(inst->Main.Argument1.ArgType) == MEMORY_TYPE &&
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
            RecursiveDisassemble(0, cpu, target, callSec, target);

            InstPtr instCalled = callSec->GetInst(target);
            Assert(instCalled != NULL);

            // auto iter = m_secMap[sect].find(target);
            // Assert(iter != m_secMap[sect].end());
            // const Instruction &instCalled = *iter->second.ptr;
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

void Disassembler::UpdateInstContext( InstContext &ctx ) const
{
    Assert(m_currProcessor);

    InstPtr inst    = m_instMem.GetInst(m_currProcessor->EIP);
    ctx.inst        = inst;
    
//     const Section *sec = m_currProcessor->Mem->GetSection(m_currProcessor->EIP);
//     auto iterSec = m_secMap.find(sec);
//     Assert(iterSec != m_secMap.end());
//     const InstDisasmMap &im = iterSec->second;
//     auto iter = im.find(m_currProcessor->EIP);
//     Assert(iter != im.end());
//     ctx.inst = iter->second;
}

// InstPtr Disassembler::GetInst( const Processor *cpu, u32 eip )
// {
//     Section *sec = cpu->Mem->GetSection(eip);
//     auto iterSec = m_secMap.find(sec);
//     Assert(iterSec != m_secMap.end());
//     auto iter = iterSec->second.find(eip);
//     Assert(iter != iterSec->second.end());
//     return iter->second;
// }

