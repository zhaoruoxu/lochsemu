#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Div_F7_ext6(const Instruction *inst)
{
    /**
     * DIV r/m32
     */
    // TODO : divide error
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    u32 val1 = ReadOperand32(inst, inst->Main.Argument2, NULL);
    if (val1 == 0) {
        // division by zero
        Exception.Raise(STATUS_INTEGER_DIVIDE_BY_ZERO);
        
    }
    u64 d = (((u64) EDX) << 32) + (u64) EAX;
    EAX = (u32) (d / val1);
    EDX = (u32) (d % val1);
    
}


END_NAMESPACE_LOCHSEMU()