#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Addsd_F20F58(const Instruction *inst)
{
    // ADDSD xmm1, xmm2/m64
    Assert(!inst->Main.Prefix.OperandSize);

    u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
    double t;
    if (IsMemoryArg(inst->Main.Argument2)) {
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        t = *((double *) &val2);
    } else if (IsRegArg(inst->Main.Argument2)) {
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        t = val2.d[0];
    } else {
        Assert(0);
    }
    val1.d[0] += t;
    WriteOperand128(inst, inst->Main.Argument1, 0, val1);
}

void Processor::Psubb_0FF8(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PSUBB xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        u8p src = (u8p) &val2, dest = (u8p) &val1;
        for (int i = 0; i < 16; i++)
            dest[i] -= src[i];
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);

    } else {
        // PSUBB mm, mm/m64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        u8p src = (u8p) &val2, dest = (u8p) &val1;
        for (int i = 0; i < 8; i++)
            dest[i] -= src[i];
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
}

void Processor::Psubw_0FF9(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PSUBW xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        u16p src = (u16p) &val2, dest = (u16p) &val1;
        for (int i = 0; i < 8; i++)
            dest[i] -= src[i];
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);

    } else {
        // PSUBW mm, mm/m64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        u16p src = (u16p) &val2, dest = (u16p) &val1;
        for (int i = 0; i < 4; i++)
            dest[i] -= src[i];
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
}

void Processor::Psubd_0FFA(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PSUBD xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        u32p src = (u32p) &val2, dest = (u32p) &val1;
        for (int i = 0; i < 4; i++)
            dest[i] -= src[i];
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);

    } else {
        // PSUBD mm, mm/m64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        u32p src = (u32p) &val2, dest = (u32p) &val1;
        for (int i = 0; i < 2; i++)
            dest[i] -= src[i];
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
}

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

void Processor::Paddw_0FFD(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PADDW xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        u16p src = (u16p) &val2, dest = (u16p) &val1;
        for (int i = 0; i < 8; i++)
            dest[i] += src[i];
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);

    } else {
        // PADDW mm, mm/m64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        u16p src = (u16p) &val2, dest = (u16p) &val1;
        for (int i = 0; i < 4; i++)
            dest[i] += src[i];
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
}

void Processor::Paddd_0FFE(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PADDD xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        u32p src = (u32p) &val2, dest = (u32p) &val1;
        for (int i = 0; i < 4; i++)
            dest[i] += src[i];
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);

    } else {
        // PADDD mm, mm/m64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        u32p src = (u32p) &val2, dest = (u32p) &val1;
        for (int i = 0; i < 2; i++)
            dest[i] += src[i];
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
}

void Processor::Pmulld_660F3840(const Instruction *inst)
{
    // pmulld xmm1, xmm2/m128
    Assert(inst->Main.Prefix.OperandSize);

    u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
    u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
    i64 temp[4];
    for (int i = 0; i < 4; i++) {
        temp[i] = PROMOTE_I64(val1.dat[i]) * PROMOTE_I64(val2.dat[i]);
        val1.dat[i] = (i32) temp[i];
    }
    WriteOperand128(inst, inst->Main.Argument1, 0, val1);
}


END_NAMESPACE_LOCHSEMU()