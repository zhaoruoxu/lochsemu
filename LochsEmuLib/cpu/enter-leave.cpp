#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Enter_C8(const Instruction *inst)
{
    /**
     * ENTER
     */
    Push32(EBP);
    u8 nest = (u8) inst->Aux.op2.immediate;
    u32 frameTemp = ESP;
    if (nest > 0) {
        NOT_IMPLEMENTED();
    }
    EBP = frameTemp;
    ESP -= (u16) inst->Aux.op1.immediate;
    
}

void Processor::Leave_C9(const Instruction *inst)
{
    /**
     * LEAVE
     */
    ESP = EBP;
    EBP = Pop32();
    
}

END_NAMESPACE_LOCHSEMU()