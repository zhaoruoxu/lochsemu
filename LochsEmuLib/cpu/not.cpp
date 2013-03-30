#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Not_F6_ext2(const Instruction *inst)
{
    /**
     * NOT r/m8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 r = ~val1;
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::Not_F7_ext2(const Instruction *inst)
{
    /**
     * NOT r/m16
     * NOT r/m32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 r = ~val1;
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 r = ~val1;
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

END_NAMESPACE_LOCHSEMU()