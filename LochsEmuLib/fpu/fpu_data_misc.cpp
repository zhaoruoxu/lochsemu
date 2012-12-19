#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fxch_D9C8(Processor *cpu, const Instruction *inst)
{
    // FXCH ST(i)
    int i = inst->Aux.opcode - 0xC8;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fxch st(0); 
            break;
        case 1: __asm fxch st(1); 
            break;
        case 2: __asm fxch st(2); 
            break;
        case 3: __asm fxch st(3); 
            break;
        case 4: __asm fxch st(4); 
            break;
        case 5: __asm fxch st(5); 
            break;
        case 6: __asm fxch st(6); 
            break;
        case 7: __asm fxch st(7); 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()