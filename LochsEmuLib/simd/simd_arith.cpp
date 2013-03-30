#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Paddb_0FFC(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PADDB xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        u8p src = (u8p) &val2, dest = (u8p) &val1;
        for (int i = 0; i < 16; i++)
            dest[i] += src[i];
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);

    } else {
        // PADDB mm, mm/m64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        u8p src = (u8p) &val2, dest = (u8p) &val1;
        for (int i = 0; i < 8; i++)
            dest[i] += src[i];
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

END_NAMESPACE_LOCHSEMU()