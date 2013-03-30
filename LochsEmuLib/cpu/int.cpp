#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Int3_CC(const Instruction *inst)
{
	//don't know how to implement (lh)
	LxFatal("INT3 instruction\n");
	
}

END_NAMESPACE_LOCHSEMU()