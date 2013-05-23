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

void Processor::Psrld_0F72_ext2(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PSRLD xmm1, imm8
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        byte imm = (byte) inst->Main.Inst.Immediat;
        val1.dat[0] >>= imm;
        val1.dat[1] >>= imm;
        val1.dat[2] >>= imm;
        val1.dat[3] >>= imm;
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);
    } else {
        // PSRLD mm, imm8
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        byte imm = (byte) inst->Main.Inst.Immediat;
        u32p dest = (u32p) &val1;
        dest[0] >>= imm;
        dest[1] >>= imm;
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
}

void Processor::Pslld_0F72_ext6(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PSLLD xmm1, imm8
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        byte imm = (byte) inst->Main.Inst.Immediat;
        val1.dat[0] <<= imm;
        val1.dat[1] <<= imm;
        val1.dat[2] <<= imm;
        val1.dat[3] <<= imm;
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);
    } else {
        // PSLLD mm, imm8
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        byte imm = (byte) inst->Main.Inst.Immediat;
        u32p dest = (u32p) &val1;
        dest[0] <<= imm;
        dest[1] <<= imm;
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
}

void Processor::Psrldq_0F73_ext3(const Instruction *inst)
{
    Assert(inst->Main.Prefix.OperandSize);

    // PSRLDQ xmm1, imm8;
    u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
    byte imm = (byte) inst->Main.Inst.Immediat;
    u128 r;
    pbyte dest = (pbyte) &r, src = (pbyte) &val1;
    if (imm <= 15) {
        for (int i = 0; i < 16 - imm; i++)
            dest[i] = src[i + imm];
    }
    WriteOperand128(inst, inst->Main.Argument1, 0, r);
}

END_NAMESPACE_LOCHSEMU()