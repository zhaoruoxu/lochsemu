#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fchs_D9E0(Processor *cpu, const Instruction *inst)
{
    // FCHS
    cpu->FPU()->RestoreContext();
    __asm fchs;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fabs_D9E1(Processor *cpu, const Instruction *inst)
{
    // FABS
    cpu->FPU()->RestoreContext();
    __asm fabs;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fyl2x_D9F1(Processor *cpu, const Instruction *inst)
{
    // FYL2X
    cpu->FPU()->RestoreContext();
    __asm fyl2x;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fptan_D9F2(Processor *cpu, const Instruction *inst)
{
    // FPTAN
    cpu->FPU()->RestoreContext();
    __asm fptan;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fprem1_D9F5(Processor *cpu, const Instruction *inst)
{
    // FPREM1
    cpu->FPU()->RestoreContext();
    __asm fprem1;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fprem_D9F8(Processor *cpu, const Instruction *inst)
{
    // FPREM
    cpu->FPU()->RestoreContext();
    __asm fprem;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fsqrt_D9FA(Processor *cpu, const Instruction *inst)
{
    // FSQRT
    cpu->FPU()->RestoreContext();
    __asm fsqrt;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Frndint_D9FC(Processor *cpu, const Instruction *inst)
{
    // FRNDINT
    cpu->FPU()->RestoreContext();
    __asm frndint;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()