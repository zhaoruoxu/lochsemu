#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Xadd_0FC1(Processor *cpu, const Instruction *inst)
{
	//XADD r/m16/32, r16/32
	if (inst->Main.Prefix.OperandSize) {
		u32 offset = 0;
		u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 r = val1 + val2;
		cpu->SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2), PROMOTE_I32(val1) + PROMOTE_I32(val2));

		cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
		cpu->WriteOperand16(inst, inst->Main.Argument2, 0, val1);
	}else {
		u32 offset = 0;
		u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
		u32 r = val1 + val2;
		cpu->SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2), PROMOTE_I64(val1) + PROMOTE_I64(val2));

		cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
		cpu->WriteOperand32(inst, inst->Main.Argument2, 0, val1);
	}
	RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()