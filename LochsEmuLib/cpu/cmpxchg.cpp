#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Cmpxchg_0FB0(const Instruction *inst)
{
    // CMPXCHG r/m8, r8
    u32 offset = 0;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    if (AL == val1) {
        ZF = 1;
        WriteOperand8(inst, inst->Main.Argument1, offset, val2);
    } else {
        ZF = 0;
        AL = val1;
    }
}

void Processor::Cmpxchg_0FB1(const Instruction *inst)
{
    /**
     * CMPXCHG r/m32, r32
     */
    u32 offset = 0;
    if (inst->Main.Prefix.OperandSize) {
        
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        if (AX == val1) {
            ZF = 1;
            WriteOperand16(inst, inst->Main.Argument1, offset, val2);
        } else {
            ZF = 0;
            AX = val1;
        }
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        if (EAX == val1) {
            ZF = 1;
            WriteOperand32(inst, inst->Main.Argument1, offset, val2);
        } else {
            ZF = 0;
            EAX = val1;
        }
    }

    
}


END_NAMESPACE_LOCHSEMU()