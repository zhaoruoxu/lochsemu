#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Lea_8D(const Instruction *inst)
{
    /**
     * LEA r16, m
     * LEA r32, m
     */

    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = (u16) Offset32(inst->Main.Argument2);
        WriteOperand16(inst, inst->Main.Argument1, 0, val2);
    } else {
        u32 val2 = Offset32(inst->Main.Argument2);
        WriteOperand32(inst, inst->Main.Argument1, 0, val2);
    }
    
}

END_NAMESPACE_LOCHSEMU()