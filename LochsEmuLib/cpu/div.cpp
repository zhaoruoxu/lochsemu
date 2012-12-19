#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Div_F7_ext6(Processor *cpu, const Instruction *inst)
{
    /**
     * DIV r/m32
     */
    // TODO : divide error
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    if (val1 == 0) {
        // division by zero
        cpu->Exception.Raise(STATUS_INTEGER_DIVIDE_BY_ZERO);
        RET_SUCCESS();
    }
    u64 d = (((u64) cpu->EDX) << 32) + (u64) cpu->EAX;
    cpu->EAX = (u32) (d / val1);
    cpu->EDX = (u32) (d % val1);
    RET_SUCCESS();
}


END_NAMESPACE_LOCHSEMU()