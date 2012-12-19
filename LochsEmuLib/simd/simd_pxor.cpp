#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Pxor_66_0FEF(Processor *cpu, const Instruction *inst)
{
	if (inst->Main.Prefix.OperandSize){
	//PXOR xmm0, xmm2/m128
		u128 val1 = cpu->ReadOperand128(inst, inst->Main.Argument1, NULL);
		u128 val2 = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
		u128 r;
		r.dat[0] = val1.dat[0] ^ val2.dat[0];
		r.dat[1] = val1.dat[1] ^ val2.dat[1];
		r.dat[2] = val1.dat[2] ^ val2.dat[2];
		r.dat[3] = val1.dat[3] ^ val2.dat[3];
		cpu->WriteOperand128(inst, inst->Main.Argument1, 0, r);
	} else {
		RET_NOT_IMPLEMENTED();
	}
	RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()