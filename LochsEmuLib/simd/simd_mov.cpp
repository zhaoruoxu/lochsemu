#include "stdafx.h"
#include "processor.h"
#include "simd.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Movapd_66_0F28(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // MOVAPD xmm1, xmm2/m128
        u128 val = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand128(inst, inst->Main.Argument1, 0, val);
    } else {
        RET_NOT_IMPLEMENTED();
    }
    RET_SUCCESS();
}

LxResult Movdqa_0F6F(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // MOVDQA
        u128 val = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand128(inst, inst->Main.Argument1, 0, val);
    } else {
        //RET_NOT_IMPLEMENTED();
        u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand64(inst, inst->Main.Argument1, 0, val);
    }
    RET_SUCCESS();
}

LxResult Movd_0F7E(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // 66 prefix, sse
        u128 val = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand32(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1),
            val.dat[0]);
    } else {
        u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand32(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1),
            (u32) val);
    }
    RET_SUCCESS();
}

LxResult Movdqa_0F7F(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {	//SSE
        // MOVDQA
        u128 val = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand128(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val);
    } else {												// MMX
        u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
		cpu->WriteOperand64(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()