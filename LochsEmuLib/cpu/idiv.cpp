#include "stdafx.h"
#include "processor.h"
#include "exception.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Idiv_F7_ext7(const Instruction *inst)
{
    /*
     * IDIV r/m32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 regEax = EAX;
    u32 regEdx = EDX;
    u32 val = ReadOperand32(inst, inst->Main.Argument2, NULL);
    if (val == 0) {
        // division by zero
        Exception.Raise(STATUS_INTEGER_DIVIDE_BY_ZERO);
        
    }
    __asm {
        mov eax, regEax
        mov edx, regEdx
        idiv val
        mov regEax, eax
        mov regEdx, edx
    }
    EAX = regEax;
    EDX = regEdx;
    
}

END_NAMESPACE_LOCHSEMU()