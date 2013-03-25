#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Xorps_0F57(Processor *cpu, const Instruction *inst)
{
    u128 val1 = cpu->ReadOperand128(inst, inst->Main.Argument1, NULL);
    u128 val2 = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
    val1.dat[0] ^= val2.dat[0];
    val1.dat[1] ^= val2.dat[1];
    val1.dat[2] ^= val2.dat[2];
    val1.dat[3] ^= val2.dat[3];
    cpu->WriteOperand128(inst, inst->Main.Argument1, 0, val1);
    RET_SUCCESS();
}

LxResult Pand_0FDB(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize){
        // PAND xmm1, xmm2/m128
        u128 val1 = cpu->ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
        val1.dat[0] &= val2.dat[0];
        val1.dat[1] &= val2.dat[1];
        val1.dat[2] &= val2.dat[2];
        val1.dat[3] &= val2.dat[3];
        cpu->WriteOperand128(inst, inst->Main.Argument1, 0, val1);
    } else {
        // PAND mm, mm/m64
        u64 val1 = cpu->ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
        val1 &= val2;
        cpu->WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
    RET_SUCCESS();
}

LxResult Pxor_0FEF(Processor *cpu, const Instruction *inst)
{
	if (inst->Main.Prefix.OperandSize){
	    // PXOR xmm1, xmm2/m128
		u128 val1 = cpu->ReadOperand128(inst, inst->Main.Argument1, NULL);
		u128 val2 = cpu->ReadOperand128(inst, inst->Main.Argument2, NULL);
		val1.dat[0] ^= val2.dat[0];
		val1.dat[1] ^= val2.dat[1];
		val1.dat[2] ^= val2.dat[2];
		val1.dat[3] ^= val2.dat[3];
		cpu->WriteOperand128(inst, inst->Main.Argument1, 0, val1);
	} else {
        // PXOR mm, mm/m64
        u64 val1 = cpu->ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
        val1 ^= val2;
        cpu->WriteOperand64(inst, inst->Main.Argument1, 0, val1);
	}
	RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()