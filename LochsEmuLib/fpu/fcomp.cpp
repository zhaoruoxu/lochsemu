#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fcom_D8D0(Processor *cpu, const Instruction *inst)
{
    // FCOM ST(0), ST(i)
    int i = inst->Aux.opcode - 0xd0;
    cpu->FPU()->RestoreContext();
    switch (i) {
    case 0: __asm fcom st(0); 
        break;
    case 1: __asm fcom st(1); 
        break;
    case 2: __asm fcom st(2); 
        break;
    case 3: __asm fcom st(3); 
        break;
    case 4: __asm fcom st(4); 
        break;
    case 5: __asm fcom st(5); 
        break;
    case 6: __asm fcom st(6); 
        break;
    case 7: __asm fcom st(7); 
        break;
    default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fucompp_DAE9(Processor *cpu, const Instruction *inst)
{
	// FUCOMPP
	cpu->FPU()->RestoreContext();
	__asm fucompp;
	cpu->FPU()->SaveContext();
	RET_SUCCESS();
}

LxResult Fpu_Fcom_DC_2(Processor *cpu, const Instruction *inst)
{
    // FCOM m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fcom      val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fcomp_DC_3(Processor *cpu, const Instruction *inst)
{
    // FCOMP m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fcomp     val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()