#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

static void JumpRel8(Processor *cpu, const Instruction *inst)
{
	u32 rel = SIGN_EXTEND(8, 32, inst->Aux.op1.immediate);
	cpu->EIP += rel;
}

LxResult Loop_E2(Processor *cpu, const Instruction *inst)
{
	// LOOP rel8
	if (inst->Main.Prefix.OperandSize) {
		return LX_RESULT_NOT_IMPLEMENTED;
	}
	cpu->ECX --;
	if (cpu->ECX != 0) {
		JumpRel8(cpu, inst);
	}
	RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()