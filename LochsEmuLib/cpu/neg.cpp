#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Neg_F6_ext3(Processor *cpu, const Instruction *inst)
{
    /**
     * NEG r/m8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 r = 0-val1;
    cpu->SetFlagsArith8(r, -PROMOTE_U16(val1), -PROMOTE_I16(val1));
    cpu->CF = (val1 == 0) ? 0 : 1;
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Neg_F7_ext3(Processor *cpu, const Instruction *inst)
{
    /**
     * NEG r/m16
     * NEG r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u32 offset;
		u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val  = 0 - val1;
		cpu->SetFlagsArith16(val, 0 - PROMOTE_U32(val1), 0 - PROMOTE_I32(val1));
		cpu->CF = (val1 == 0) ? 0 : 1;
		cpu->WriteOperand16(inst, inst->Main.Argument1, offset, val);
    } else {
        u32 offset;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 r = 0-val1;
        cpu->SetFlagsArith32(r, 0-PROMOTE_U64(val1), 0-PROMOTE_I64(val1));
        cpu->CF = (val1 == 0) ? 0 : 1;
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()