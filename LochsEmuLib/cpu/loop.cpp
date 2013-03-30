#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::JumpRel8(const Instruction *inst)
{
	u32 rel = SIGN_EXTEND(8, 32, inst->Aux.op1.immediate);
	EIP += rel;
}

void Processor::Loop_E2(const Instruction *inst)
{
	// LOOP rel8
	if (inst->Main.Prefix.OperandSize) {
		NOT_IMPLEMENTED();
	}
	ECX --;
	if (ECX != 0) {
		JumpRel8(inst);
	}
	
}

END_NAMESPACE_LOCHSEMU()