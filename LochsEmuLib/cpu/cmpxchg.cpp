#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Cmpxchg_0FB1(Processor *cpu, const Instruction *inst)
{
    /**
     * CMPXCHG r/m32, r32
     */
    u32 offset = 0;
    if (inst->Main.Prefix.OperandSize) {
        
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
        if (cpu->AX == val1) {
            cpu->ZF = 1;
            cpu->WriteOperand16(inst, inst->Main.Argument1, offset, val2);
        } else {
            cpu->ZF = 0;
            cpu->AX = val1;
        }
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        if (cpu->EAX == val1) {
            cpu->ZF = 1;
            cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val2);
        } else {
            cpu->ZF = 0;
            cpu->EAX = val1;
        }
    }

    RET_SUCCESS();
}


END_NAMESPACE_LOCHSEMU()