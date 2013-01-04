#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fsub_D8E0(Processor *cpu, const Instruction *inst)
{
    // FSUB ST(0), ST(i)
    int i = inst->Aux.opcode - 0xe0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fsub st, st(0); 
            break;
        case 1: __asm fsub st, st(1); 
            break;
        case 2: __asm fsub st, st(2); 
            break;
        case 3: __asm fsub st, st(3); 
            break;
        case 4: __asm fsub st, st(4); 
            break;
        case 5: __asm fsub st, st(5); 
            break;
        case 6: __asm fsub st, st(6); 
            break;
        case 7: __asm fsub st, st(7); 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fsubr_D8E8(Processor *cpu, const Instruction *inst)
{
    // FSUBR ST(0), ST(i)
    int i = inst->Aux.opcode - 0xe8;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fsubr st, st(0); 
            break;
        case 1: __asm fsubr st, st(1); 
            break;
        case 2: __asm fsubr st, st(2); 
            break;
        case 3: __asm fsubr st, st(3); 
            break;
        case 4: __asm fsubr st, st(4); 
            break;
        case 5: __asm fsubr st, st(5); 
            break;
        case 6: __asm fsubr st, st(6); 
            break;
        case 7: __asm fsubr st, st(7); 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fsub_DC_4(Processor *cpu, const Instruction *inst)
{
    // FSUB m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fsub      val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fsubr_DC_5(Processor *cpu, const Instruction *inst)
{
    // FSUBR m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fsubr     val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fsubr_DCE0(Processor *cpu, const Instruction *inst)
{
    // FSUBR ST(i), ST(0)
    int i = inst->Aux.opcode - 0xe0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fsubr st(0), st; 
            break;
        case 1: __asm fsubr st(1), st; 
            break;
        case 2: __asm fsubr st(2), st; 
            break;
        case 3: __asm fsubr st(3), st; 
            break;
        case 4: __asm fsubr st(4), st; 
            break;
        case 5: __asm fsubr st(5), st; 
            break;
        case 6: __asm fsubr st(6), st; 
            break;
        case 7: __asm fsubr st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fsubrp_DEE0(Processor *cpu, const Instruction *inst)
{
    // FSUBRP ST(i), ST(0)
    int i = inst->Aux.opcode - 0xe0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fsubrp st(0), st; 
            break;
        case 1: __asm fsubrp st(1), st; 
            break;
        case 2: __asm fsubrp st(2), st; 
            break;
        case 3: __asm fsubrp st(3), st; 
            break;
        case 4: __asm fsubrp st(4), st; 
            break;
        case 5: __asm fsubrp st(5), st; 
            break;
        case 6: __asm fsubrp st(6), st; 
            break;
        case 7: __asm fsubrp st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fsubp_DEE8(Processor *cpu, const Instruction *inst)
{
    // FSUBP ST(i), ST(0)
    int i = inst->Aux.opcode - 0xe8;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fsubp st(0), st; 
            break;
        case 1: __asm fsubp st(1), st; 
            break;
        case 2: __asm fsubp st(2), st; 
            break;
        case 3: __asm fsubp st(3), st; 
            break;
        case 4: __asm fsubp st(4), st; 
            break;
        case 5: __asm fsubp st(5), st; 
            break;
        case 6: __asm fsubp st(6), st; 
            break;
        case 7: __asm fsubp st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();

    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()