#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Lea_8D(Processor *cpu, const Instruction *inst)
{
    /**
     * LEA r16, m
     * LEA r32, m
     */

    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = (u16) cpu->Offset32(inst->Main.Argument2);
        cpu->WriteOperand16(inst, inst->Main.Argument1, 0, val2);
    } else {
        u32 val2 = cpu->Offset32(inst->Main.Argument2);
        cpu->WriteOperand32(inst, inst->Main.Argument1, 0, val2);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()