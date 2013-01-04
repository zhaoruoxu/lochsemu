#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fadd32fp_D8_0(Processor *cpu, const Instruction *inst)
{
	// FADD m32fp
	u32 val = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
	cpu->FPU()->RestoreContext();
	__asm fadd val;
	cpu->FPU()->SaveContext();
	RET_SUCCESS();
}

LxResult Fpu_Fadd_D8C0(Processor *cpu, const Instruction *inst)
{
    // FADD ST(0), ST(i)
    int i = inst->Aux.opcode - 0xc0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fadd st, st(0); 
            break;
        case 1: __asm fadd st, st(1); 
            break;
        case 2: __asm fadd st, st(2); 
            break;
        case 3: __asm fadd st, st(3); 
            break;
        case 4: __asm fadd st, st(4); 
            break;
        case 5: __asm fadd st, st(5); 
            break;
        case 6: __asm fadd st, st(6); 
            break;
        case 7: __asm fadd st, st(7); 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fadd_DC_0(Processor *cpu, const Instruction *inst)
{
    // FADD m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fadd  val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fadd_DCC0(Processor *cpu, const Instruction *inst)
{
    // FADD ST(i), ST(0)
    int i = inst->Aux.opcode - 0xc0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fadd st(0), st; 
            break;
        case 1: __asm fadd st(1), st; 
            break;
        case 2: __asm fadd st(2), st; 
            break;
        case 3: __asm fadd st(3), st; 
            break;
        case 4: __asm fadd st(4), st; 
            break;
        case 5: __asm fadd st(5), st; 
            break;
        case 6: __asm fadd st(6), st; 
            break;
        case 7: __asm fadd st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();

}

LxResult Fpu_Faddp_DEC0(Processor *cpu, const Instruction *inst)
{
    // FADDP ST(i), ST(0)
    int i = inst->Aux.opcode - 0xc0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm faddp st(0), st; 
            break;
        case 1: __asm faddp st(1), st; 
            break;
        case 2: __asm faddp st(2), st; 
            break;
        case 3: __asm faddp st(3), st; 
            break;
        case 4: __asm faddp st(4), st; 
            break;
        case 5: __asm faddp st(5), st; 
            break;
        case 6: __asm faddp st(6), st; 
            break;
        case 7: __asm faddp st(7), st; 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}   

END_NAMESPACE_LOCHSEMU()