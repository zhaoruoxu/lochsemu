#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Cdq_99(const Instruction *inst)
{
    /*
     * CDQ
     */
    if (inst->Main.Prefix.OperandSize)
        NOT_IMPLEMENTED();
    u32 val1 = EAX;
    u64 r = SIGN_EXTEND(32, 64, val1);
    EAX = (u32) r;
    EDX = (u32) (r >> 32);
    
}

END_NAMESPACE_LOCHSEMU()