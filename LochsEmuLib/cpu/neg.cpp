#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Neg_F6_ext3(const Instruction *inst)
{
    /**
     * NEG r/m8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 r = 0-val1;
    SetFlagsArith8(r, -PROMOTE_U16(val1), -PROMOTE_I16(val1));
    CF = (val1 == 0) ? 0 : 1;
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::Neg_F7_ext3(const Instruction *inst)
{
    /**
     * NEG r/m16
     * NEG r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u32 offset;
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val  = 0 - val1;
		SetFlagsArith16(val, 0 - PROMOTE_U32(val1), 0 - PROMOTE_I32(val1));
		CF = (val1 == 0) ? 0 : 1;
		WriteOperand16(inst, inst->Main.Argument1, offset, val);
    } else {
        u32 offset;
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 r = 0-val1;
        SetFlagsArith32(r, 0-PROMOTE_U64(val1), 0-PROMOTE_I64(val1));
        CF = (val1 == 0) ? 0 : 1;
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

END_NAMESPACE_LOCHSEMU()