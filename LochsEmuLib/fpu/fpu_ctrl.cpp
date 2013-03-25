#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fxam_D9E5(Processor *cpu, const Instruction *inst)
{
    // FXAM
    cpu->FPU()->RestoreContext();
    __asm fxam;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fnclex_DBE2(Processor *cpu, const Instruction *inst)
{
    // FNCLEX
    //LxWarning("FPU: Fnclex\n");
    RET_SUCCESS();
}

LxResult Fpu_Fninit_DBE3(Processor *cpu, const Instruction *inst)
{ 
    // FNINIT
    cpu->FPU()->Reset();
    RET_SUCCESS();
}

LxResult Fpu_Wait_9B(Processor *cpu, const Instruction *inst)
{
    // WAIT/FWAIT
    cpu->FPU()->Wait();
    RET_SUCCESS();
}

LxResult Emms_0F77(Processor *cpu, const Instruction *inst)
{
    cpu->FPU()->RestoreContext();
    __asm emms;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()