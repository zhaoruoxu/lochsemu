#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Enter_C8(Processor *cpu, const Instruction *inst)
{
    /**
     * ENTER
     */
    cpu->Push32(cpu->EBP);
    u8 nest = (u8) inst->Aux.op2.immediate;
    u32 frameTemp = cpu->ESP;
    if (nest > 0) {
        RET_NOT_IMPLEMENTED();
    }
    cpu->EBP = frameTemp;
    cpu->ESP -= (u16) inst->Aux.op1.immediate;
    RET_SUCCESS();
}

LxResult Leave_C9(Processor *cpu, const Instruction *inst)
{
    /**
     * LEAVE
     */
    cpu->ESP = cpu->EBP;
    cpu->EBP = cpu->Pop32();
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()