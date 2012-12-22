#include "stdafx.h"
#include "engine.h"
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

MemoryTaint::MemoryTaint( )
{

}

Taint MemoryTaint::Get( u32 addr )
{
    if (m_status.find(addr) == m_status.end()) {
        m_status[addr] = Taint();
    }
    return m_status[addr];
}

Taint MemoryTaint::Get( u32 addr, u32 len )
{
    Taint ret = Get(addr);
    for (uint i = 1; i < len; i++) {
        ret |= Get(addr + i);
    }
    return ret;
}

void MemoryTaint::Set( const Taint &t, u32 addr, u32 len /*= 1*/ )
{
    Assert(len > 0);
    for (uint i = 0; i < len; i++) {
        m_status[addr + i] = t;
    }
}


TaintEngine::TaintEngine()
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
        return m_cpuTaint.GPRegs[index];
    } else if (OPERAND_TYPE(oper.ArgType) == MEMORY_TYPE) {
        u32 o = cpu->Offset32(oper);
        return m_memTaint.Get(o, oper.ArgSize / 8);
    } else {
        return Taint();
    }
}

void TaintEngine::SetTaint( const Processor *cpu, const ARGTYPE &oper, const Taint &t )
{
    Assert(t.GetIndices() > 0);
    if (OPERAND_TYPE(oper.ArgType) == REGISTER_TYPE) {
        m_cpuTaint.GPRegs[RegMap[REG_NUM(oper.ArgType)]] = t;
    } else if (OPERAND_TYPE(oper.ArgType) == MEMORY_TYPE) {
        u32 o = cpu->Offset32(oper);
        return m_memTaint.Set(t, o, oper.ArgSize / 8);
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
