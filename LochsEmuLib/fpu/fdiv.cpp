#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fdiv32fp_D8_6(Processor *cpu, const Instruction *inst)
{
    // FDIV m32fp
    u32 val = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fdiv  val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fdiv_D8F0(Processor *cpu, const Instruction *inst)
{
    // FDIV st(0), st(i)
    int i = inst->Aux.opcode - 0xf0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fdiv st, st(0); 
            break;
        case 1: __asm fdiv st, st(1); 
            break;
        case 2: __asm fdiv st, st(2); 
            break;
        case 3: __asm fdiv st, st(3); 
            break;
        case 4: __asm fdiv st, st(4); 
            break;
        case 5: __asm fdiv st, st(5); 
            break;
        case 6: __asm fdiv st, st(6); 
            break;
        case 7: __asm fdiv st, st(7); 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();

}

LxResult Fpu_Fidiv_DA_6(Processor *cpu, const Instruction *inst)
{
    // FIDIV m32int
    u32 val = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fidiv     val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fidiv_DE_6(Processor *cpu, const Instruction *inst)
{
    // FIDIV m64int
    u16 val = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fidiv     val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fdiv_DC_6(Processor *cpu, const Instruction *inst)
{
    // FDIV m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fdiv  val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fdivr_DC_7(Processor *cpu, const Instruction *inst)
{
    // FDIVR m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fdivr     val;
    cpu->FPU()->SaveContext();

    RET_SUCCESS();
}

LxResult Fpu_Fdivr_DCF0(Processor *cpu, const Instruction *inst)
{
    // FDIVR st(i), st(0)
    int i = inst->Aux.opcode - 0xF0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fdivr st(0), st; 
            break;
        case 1: __asm fdivr st(1), st; 
            break;
        case 2: __asm fdivr st(2), st; 
            break;
        case 3: __asm fdivr st(3), st; 
            break;
        case 4: __asm fdivr st(4), st; 
            break;
        case 5: __asm fdivr st(5), st; 
            break;
        case 6: __asm fdivr st(6), st; 
            break;
        case 7: __asm fdivr st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fdiv_DCF8(Processor *cpu, const Instruction *inst)
{
    // FDIV st(i), st(0)
    int i = inst->Aux.opcode - 0xF8;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fdiv st(0), st; 
            break;
        case 1: __asm fdiv st(1), st; 
            break;
        case 2: __asm fdiv st(2), st; 
            break;
        case 3: __asm fdiv st(3), st; 
            break;
        case 4: __asm fdiv st(4), st; 
            break;
        case 5: __asm fdiv st(5), st; 
            break;
        case 6: __asm fdiv st(6), st; 
            break;
        case 7: __asm fdiv st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();

}

LxResult Fpu_Fdivp_DEF8(Processor *cpu, const Instruction *inst)
{
    // FDIVP st(i), st(0)
    int i = inst->Aux.opcode - 0xF8;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fdivp st(0), st; 
            break;
        case 1: __asm fdivp st(1), st; 
            break;
        case 2: __asm fdivp st(2), st; 
            break;
        case 3: __asm fdivp st(3), st; 
            break;
        case 4: __asm fdivp st(4), st; 
            break;
        case 5: __asm fdivp st(5), st; 
            break;
        case 6: __asm fdivp st(6), st; 
            break;
        case 7: __asm fdivp st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();

}

END_NAMESPACE_LOCHSEMU()