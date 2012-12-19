#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Cdq_99(Processor *cpu, const Instruction *inst)
{
    /*
     * CDQ
     */
    if (inst->Main.Prefix.OperandSize)
        RET_NOT_IMPLEMENTED();
    u32 val1 = cpu->EAX;
    u64 r = SIGN_EXTEND(32, 64, val1);
    cpu->EAX = (u32) r;
    cpu->EDX = (u32) (r >> 32);
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()