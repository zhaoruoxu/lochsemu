#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Cbw_98(const Instruction *inst)
{
	// CBW / CWDE
	if(inst->Main.Prefix.OperandSize){ // CBW
		u8 val1 = AL;
		u16 r = SIGN_EXTEND(8, 16, val1);
		AX = r;
	} else { // CWDE
		u16 val1 = AX;
		u32 r = SIGN_EXTEND(16, 32, val1);
		EAX = r;
	}
	
}

END_NAMESPACE_LOCHSEMU()