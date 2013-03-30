#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Fpu_Fchs_D9E0(const Instruction *inst)
{
    // FCHS
    FPU()->RestoreContext();
    __asm fchs;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fabs_D9E1(const Instruction *inst)
{
    // FABS
    FPU()->RestoreContext();
    __asm fabs;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fyl2x_D9F1(const Instruction *inst)
{
    // FYL2X
    FPU()->RestoreContext();
    __asm fyl2x;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fptan_D9F2(const Instruction *inst)
{
    // FPTAN
    FPU()->RestoreContext();
    __asm fptan;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fprem1_D9F5(const Instruction *inst)
{
    // FPREM1
    FPU()->RestoreContext();
    __asm fprem1;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fprem_D9F8(const Instruction *inst)
{
    // FPREM
    FPU()->RestoreContext();
    __asm fprem;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fsqrt_D9FA(const Instruction *inst)
{
    // FSQRT
    FPU()->RestoreContext();
    __asm fsqrt;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Frndint_D9FC(const Instruction *inst)
{
    // FRNDINT
    FPU()->RestoreContext();
    __asm frndint;
    FPU()->SaveContext();
    
}

END_NAMESPACE_LOCHSEMU()