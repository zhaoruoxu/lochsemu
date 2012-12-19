#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Cbw_98(Processor *cpu, const Instruction *inst)
{
	// CBW / CWDE
	if(inst->Main.Prefix.OperandSize){ // CBW
		u8 val1 = cpu->AL;
		u16 r = SIGN_EXTEND(8, 16, val1);
		cpu->AX = r;
	} else { // CWDE
		u16 val1 = cpu->AX;
		u32 r = SIGN_EXTEND(16, 32, val1);
		cpu->EAX = r;
	}
	RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()