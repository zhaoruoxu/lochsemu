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
    -1,  0,  4, -1,  8,  0, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1,
    16, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    20, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    24, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    28
};

MemoryTaint::MemoryTaint( )
{

}

Taint* MemoryTaint::Get( u32 addr )
{
    if (m_status.find(addr) == m_status.end()) {
        m_status[addr] = Taint();
    }
    return &m_status[addr];
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
    int len = min(arg1.ArgSize, arg2.ArgSize) / 8;
    for (int i = 0; i < len; i++) {
        Taint *t = GetTaint(cpu, inst, arg1, i);
        Taint *s = GetTaint(cpu, inst, arg2, i);
        *t |= *s;
        if (t->IsTainted(DEBUG_TAINT)) {
            LxInfo("%s  %s is propagated\n", inst->Main.CompleteInstr, arg1.ArgMnemonic);
        }
    }

    if (strstr(inst->Main.Inst.Mnemonic, "add")) {
        int len = arg1.ArgSize / 8;
        for (int i = 0; i < len; i++) {
            Taint *t = GetTaint(cpu, inst, arg1, i);
            t->Set(DEBUG_TAINT);
            LxInfo("%s  %s is introduced\n", inst->Main.CompleteInstr, arg1.ArgMnemonic);
        }
    }
}

Taint* TaintEngine::GetTaint(const Processor *cpu, const Instruction *inst, 
                            const ARGTYPE &oper, int offset )
{
    if (OPERAND_TYPE(oper.ArgType) == REGISTER_TYPE) {
        return &m_cpuTaint.GPRegs[RegMap[REG_NUM(oper.ArgType)] + oper.ArgPosition + offset];
    } else if (OPERAND_TYPE(oper.ArgType) == MEMORY_TYPE) {
        u32 o = cpu->Offset32(oper) + offset;
        return m_memTaint.Get(o);
    } else {
        // nothing to do
        return NULL;
    }
}
