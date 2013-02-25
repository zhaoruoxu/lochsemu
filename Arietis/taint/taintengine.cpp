#include "stdafx.h"
#include "taintengine.h"
#include "processor.h"

/*
 REG0 = 0x1,    -> 0
 REG1 = 0x2,    -> 1
 REG2 = 0x4,    -> 2
 REG3 = 0x8,    -> 3
 REG4 = 0x10,   -> 4
 REG5 = 0x20,   -> 5
 REG6 = 0x40,   -> 6
 REG7 = 0x80,   -> 7

 REG0 | REG2    -> 0        // TODO
 */

static int RegMap[]  = {
    -1,  0,  1, -1,  2,  0, -1, -1,  3, -1, -1, -1, -1, -1, -1, -1,
     4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     7
};

MemoryTaint::MemoryTaint() 
{
    ZeroMemory(m_pagetable, sizeof(m_pagetable));
}

MemoryTaint::~MemoryTaint()
{
    for (int i = 0; i < LX_PAGE_COUNT; i++) {
        if (m_pagetable[i]) {
            SAFE_DELETE(m_pagetable[i]);
        }
    }
}

Taint MemoryTaint::Get( u32 addr )
{
//     if (m_status.find(addr) == m_status.end()) {
//         m_status[addr] = Taint();
//     }
//     return m_status[addr];
    PageTaint *page = GetPage(addr);
    return page->Get(PAGE_LOW(addr));
}

Taint MemoryTaint::Get( u32 addr, u32 len )
{
    Taint ret = Get(addr);
    for (uint i = 1; i < len; i++) {
        ret |= Get(addr + i);
    }
    return ret;
}

void MemoryTaint::Set( const Taint &t, u32 addr, u32 len )
{
    Assert(len > 0);
    for (uint i = 0; i < len; i++) {
        //m_status[addr + i] = t;
        PageTaint *page = GetPage(addr);
        page->Set(PAGE_LOW(addr + i), t);
    }
}

MemoryTaint::PageTaint * MemoryTaint::GetPage( u32 addr )
{
    const u32 pageNum = PAGE_NUM(addr);
    if (m_pagetable[pageNum] == NULL) {
        m_pagetable[pageNum] = new PageTaint;
    }
    return m_pagetable[pageNum];
}

MemoryTaint::PageTaint::PageTaint()
{
    ZeroMemory(m_data, sizeof(m_data));
}

MemoryTaint::PageTaint::~PageTaint()
{

}

Taint MemoryTaint::PageTaint::Get( u32 offset )
{
    Assert(offset < LX_PAGE_SIZE);
    return m_data[offset];
}

void MemoryTaint::PageTaint::Set( u32 offset, const Taint &t )
{
    Assert(offset < LX_PAGE_SIZE);
    m_data[offset] = t;
}


TaintEngine::TaintEngine()
    : m_mutex(false)
{

}

TaintEngine::~TaintEngine()
{

}

void TaintEngine::Initialize()
{

}


void TaintEngine::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    #define  DEBUG_TAINT    0
    const ARGTYPE &arg1 = inst->Main.Argument1;
    const ARGTYPE &arg2 = inst->Main.Argument2;
    if (arg1.ArgType == NO_ARGUMENT || arg2.ArgType == NO_ARGUMENT ||
        OPERAND_TYPE(arg2.ArgType) == CONSTANT_TYPE ||
        inst->Main.Argument3.ArgType != NO_ARGUMENT) return;

    if (INST_TYPE(inst->Main.Inst.Category) != ARITHMETIC_INSTRUCTION)
        return;

    if (strstr(inst->Main.Inst.Mnemonic, "cmp")) return;

    DefaultTaintPropagate(cpu, inst);
    if (strstr(inst->Main.Inst.Mnemonic, "add")) {
        Taint t;
        t.Set(DEBUG_TAINT);
        SetTaint(cpu, arg1, t);
    }
}


Taint TaintEngine::GetTaint( const Processor *cpu, const ARGTYPE &oper )
{
    if (OPERAND_TYPE(oper.ArgType) == REGISTER_TYPE) {
        int index = RegMap[REG_NUM(oper.ArgType)];
        return CpuTaint.GPRegs[index];
    } else if (OPERAND_TYPE(oper.ArgType) == MEMORY_TYPE) {
        u32 o = cpu->Offset32(oper);
        return MemTaint.Get(o, oper.ArgSize / 8);
    } else {
        return Taint();
    }
}

void TaintEngine::SetTaint( const Processor *cpu, const ARGTYPE &oper, const Taint &t )
{
    //Assert(t.GetIndices() > 0);
    if (OPERAND_TYPE(oper.ArgType) == REGISTER_TYPE) {
        CpuTaint.GPRegs[RegMap[REG_NUM(oper.ArgType)]] = t;
    } else if (OPERAND_TYPE(oper.ArgType) == MEMORY_TYPE) {
        u32 o = cpu->Offset32(oper);
        return MemTaint.Set(t, o, oper.ArgSize / 8);
    } else {

    }
}


void TaintEngine::DefaultTaintPropagate( Processor *cpu, const Instruction *inst )
{
    // op1 = op1 op op2
    const ARGTYPE &arg1 = inst->Main.Argument1;
    const ARGTYPE &arg2 = inst->Main.Argument2;

    if (OPERAND_TYPE(arg2.ArgType) == CONSTANT_TYPE) return;
    Taint t = GetTaint(cpu, arg1) | GetTaint(cpu, arg2);
    SetTaint(cpu, arg1, t);
}

void TaintEngine::Lock() const
{
    m_mutex.Wait();
}

void TaintEngine::Unlock() const
{
    m_mutex.Release();
}

void TaintEngine::UpdateInstContext( InstContext *ctx ) const
{
    for (int i = InstContext::RegIndexGP; i < InstContext::RegIndexGP + InstContext::RegCount; i++)
        ctx->regTaint[i]    = CpuTaint.GPRegs[i];
    ctx->regTaint[InstContext::RegIndexEip]  = CpuTaint.Eip;
    for (int i = 0; i < InstContext::FlagCount; i++)
        ctx->flagTaint[i]   = CpuTaint.Flags[i];
}

