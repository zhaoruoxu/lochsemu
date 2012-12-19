#include "stdafx.h"
#include "processor.h"
#include "exception.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Idiv_F7_ext7(Processor *cpu, const Instruction *inst)
{
    /*
     * IDIV r/m32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 regEax = cpu->EAX;
    u32 regEdx = cpu->EDX;
    u32 val = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    if (val == 0) {
        // division by zero
        cpu->Exception.Raise(STATUS_INTEGER_DIVIDE_BY_ZERO);
        RET_SUCCESS();
    }
    __asm {
        mov eax, regEax
        mov edx, regEdx
        idiv val
        mov regEax, eax
        mov regEdx, edx
    }
    cpu->EAX = regEax;
    cpu->EDX = regEdx;
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()