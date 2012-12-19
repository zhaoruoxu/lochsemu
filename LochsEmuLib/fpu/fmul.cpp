#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fmul_D8C8(Processor *cpu, const Instruction *inst)
{
    // FMUL ST(0), ST(i)
    int i = inst->Aux.opcode - 0xc8;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fmul st, st(0); 
            break;
        case 1: __asm fmul st, st(1); 
            break;
        case 2: __asm fmul st, st(2); 
            break;
        case 3: __asm fmul st, st(3); 
            break;
        case 4: __asm fmul st, st(4); 
            break;
        case 5: __asm fmul st, st(5); 
            break;
        case 6: __asm fmul st, st(6); 
            break;
        case 7: __asm fmul st, st(7); 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();

}

LxResult Fpu_Fmul_DC_1(Processor *cpu, const Instruction *inst)
{
    // FMUL m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fmul  val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fmul_DCC8(Processor *cpu, const Instruction *inst)
{
    // FMUL ST(i), ST(0)
    int i = inst->Aux.opcode - 0xc8;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fmul st(0), st; 
            break;
        case 1: __asm fmul st(1), st; 
            break;
        case 2: __asm fmul st(2), st; 
            break;
        case 3: __asm fmul st(3), st; 
            break;
        case 4: __asm fmul st(4), st; 
            break;
        case 5: __asm fmul st(5), st; 
            break;
        case 6: __asm fmul st(6), st; 
            break;
        case 7: __asm fmul st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fmulp_DEC8(Processor *cpu, const Instruction *inst)
{
    // FMUL st(i), st(0)
    int i = inst->Aux.opcode - 0xC8;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fmulp st(0), st; 
            break;
        case 1: __asm fmulp st(1), st; 
            break;
        case 2: __asm fmulp st(2), st; 
            break;
        case 3: __asm fmulp st(3), st; 
            break;
        case 4: __asm fmulp st(4), st; 
            break;
        case 5: __asm fmulp st(5), st; 
            break;
        case 6: __asm fmulp st(6), st; 
            break;
        case 7: __asm fmulp st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();

}

END_NAMESPACE_LOCHSEMU()