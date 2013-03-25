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

LxResult Movd_0F6E(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // MOVD xmm, r/m32
        u32 val = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        u128 r;
        r.dat[0] = val;
        cpu->WriteOperand128(inst, inst->Main.Argument1, 0, r);
    } else {
        // MOVD mm, r/m32
        u32 val = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        u64 r = val;
        cpu->WriteOperand64(inst, inst->Main.Argument1, 0, r);
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
    if (inst->Main.Prefix.RepPrefix) {
        // MOVQ F30F7E
        if (IsMemoryArg(inst->Main.Argument2)) {
            u64 r = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
            u128 val;
            memcpy(&val, &r, sizeof(u64));
            cpu->WriteOperand128(inst, inst->Main.Argument1, 0, val);
        } else {
            Assert(IsRegArg(inst->Main.Argument1));
            u128 val1 = cpu->ReadOperand128(inst, inst->Main.Argument1, NULL);
            u128 val2 = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
            val1.dat[0] = val2.dat[0];
            val1.dat[1] = val2.dat[1];
            cpu->WriteOperand128(inst, inst->Main.Argument1, 0, val1);
        } 
    } else if (inst->Main.Prefix.OperandSize) {
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

LxResult Movq_0FD6(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // MOVQ xmm2/m64, xmm1
        u128 val = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
        if (IsMemoryArg(inst->Main.Argument1)) {
            u64 r = reinterpret_cast<u64p>(&val)[0];
            cpu->WriteOperand64(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), r);
        } else {
            Assert(IsRegArg(inst->Main.Argument1));
            u128 r = cpu->ReadOperand128(inst, inst->Main.Argument1, NULL);
            r.dat[0] = val.dat[0];
            r.dat[1] = val.dat[1];
            r.dat[2] = r.dat[3] = 0;
            cpu->WriteOperand128(inst, inst->Main.Argument1, 0, r);
        }
    } else {
        LxFatal("shit happens\n");
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()