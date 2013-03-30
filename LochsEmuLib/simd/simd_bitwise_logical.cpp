#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Xorps_0F57(const Instruction *inst)
{
    u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
    u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
    val1.dat[0] ^= val2.dat[0];
    val1.dat[1] ^= val2.dat[1];
    val1.dat[2] ^= val2.dat[2];
    val1.dat[3] ^= val2.dat[3];
    WriteOperand128(inst, inst->Main.Argument1, 0, val1);
    
}

void Processor::Pand_0FDB(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize){
        // PAND xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        val1.dat[0] &= val2.dat[0];
        val1.dat[1] &= val2.dat[1];
        val1.dat[2] &= val2.dat[2];
        val1.dat[3] &= val2.dat[3];
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);
    } else {
        // PAND mm, mm/m64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        val1 &= val2;
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
    
}

void Processor::Pxor_0FEF(const Instruction *inst)
{
	if (inst->Main.Prefix.OperandSize){
	    // PXOR xmm1, xmm2/m128
		u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
		u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
		val1.dat[0] ^= val2.dat[0];
		val1.dat[1] ^= val2.dat[1];
		val1.dat[2] ^= val2.dat[2];
		val1.dat[3] ^= val2.dat[3];
		WriteOperand128(inst, inst->Main.Argument1, 0, val1);
	} else {
        // PXOR mm, mm/m64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        val1 ^= val2;
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
	}
	
}

END_NAMESPACE_LOCHSEMU()