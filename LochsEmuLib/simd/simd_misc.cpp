#include "stdafx.h"
#include "processor.h"
#include "simd.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Stmxcsr_0FAE_3(Processor *cpu, const Instruction *inst)
{
    // STMXCSR
    u32 offset = cpu->Offset32(inst->Main.Argument1);
    u32 mxcsr = cpu->SIMD.MXCSR;
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, mxcsr);
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()