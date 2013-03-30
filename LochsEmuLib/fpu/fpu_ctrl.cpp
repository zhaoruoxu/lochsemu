#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Fpu_Fxam_D9E5(const Instruction *inst)
{
    // FXAM
    FPU()->RestoreContext();
    __asm fxam;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fnclex_DBE2(const Instruction *inst)
{
    // FNCLEX
    //LxWarning("FPU: Fnclex\n");
    
}

void Processor::Fpu_Fninit_DBE3(const Instruction *inst)
{ 
    // FNINIT
    FPU()->Reset();
    
}

void Processor::Fpu_Wait_9B(const Instruction *inst)
{
    // WAIT/FWAIT
    FPU()->Wait();
    
}

void Processor::Emms_0F77(const Instruction *inst)
{
    FPU()->RestoreContext();
    __asm emms;
    FPU()->SaveContext();
    
}

END_NAMESPACE_LOCHSEMU()