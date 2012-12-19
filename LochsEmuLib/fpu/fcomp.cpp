#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

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