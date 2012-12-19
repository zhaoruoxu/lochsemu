#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Int3_CC(Processor *cpu, const Instruction *inst)
{
	//don't know how to implement (lh)
	LxFatal("INT3 instruction\n");
	RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()