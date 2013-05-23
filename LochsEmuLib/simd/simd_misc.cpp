#include "stdafx.h"
#include "processor.h"
#include "simd.h"
#include <mmintrin.h>

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Comisd_0F2F(const Instruction *inst)
{
    // COMISD xmm1, xmm2/m128
    Assert(inst->Main.Prefix.OperandSize);

    double val1, val2;
    u128 t1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
    val1 = t1.d[0];
    if (IsMemoryArg(inst->Main.Argument2)) {
        u64 t2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        val2 = *((double *) &t2);
    } else if (IsRegArg(inst->Main.Argument2)) {
        u128 t2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        val2 = t2.d[0];
    } else {
        Assert(0);
    }
    OF = SF = AF = 0;
    if (_isnan(val1) || _isnan(val2)) {
        ZF = PF = CF = 1;
    } else if (val1 > val2) {
        ZF = PF = CF = 0;
    } else if (val1 < val2) {
        ZF = PF = 0; CF = 1;
    } else {
        ZF = 1; PF = CF = 0;
    }
}

void Processor::Punpckldq_0F62(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PUNPCKLDQ xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        u128 r;
        r.dat[0] = val1.dat[0];
        r.dat[1] = val2.dat[0];
        r.dat[2] = val1.dat[1];
        r.dat[3] = val2.dat[1];
        WriteOperand128(inst, inst->Main.Argument1, 0, r);
    } else {
        // PUNPCKLDQ mm, mm/m32
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        u64 r = (val1 & 0xffffffff) | (val2 << 32);
        WriteOperand64(inst, inst->Main.Argument1, 0, r);
    }
    
}

void Processor::Pcmpgtb_0F64(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // PCMPGTB xmm1, xmm2/m128
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
        u8p src = (u8p) &val2, dest = (u8p) &val1;
        for (int i = 0; i < 16; i++)
            dest[i] = (dest[i] > src[i]) ? 0xff : 0;
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);
    } else {
        // PCMPGTB mm, mm/64
        u64 val1 = ReadOperand64(inst, inst->Main.Argument1, NULL);
        u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        u8p src = (u8p) &val2, dest = (u8p) &val1;
        for (int i = 0; i < 8; i++)
            dest[i] = (dest[i] > src[i]) ? 0xff : 0;
        WriteOperand64(inst, inst->Main.Argument1, 0, val1);
    }
    
}

void Processor::Pshufw_0F70(const Instruction *inst)
{
    // PSHUFW mm1, mm2/m64, imm8
    if (inst->Main.Prefix.OperandSize) {
        // PSHUFD xmm1, xmm2/m128, imm8
        u128 val2   = ReadOperand128(inst, inst->Main.Argument2, NULL);
        byte n      = (byte) inst->Main.Inst.Immediat;
        u128 res;
        u32p src = (u32p) &val2, dest = (u32p) &res;
        dest[0]     = src[n & 3];
        dest[1]     = src[(n>>2) & 3];
        dest[2]     = src[(n>>4) & 3];
        dest[3]     = src[(n>>6) & 3];
        WriteOperand128(inst, inst->Main.Argument1, 0, res);
    } else {
        u64 val2    = ReadOperand64(inst, inst->Main.Argument2, NULL);
        byte n      = (byte) inst->Main.Inst.Immediat;
        u64 res;
        u16p src = (u16p) &val2, dest = (u16p) &res;
        dest[0]     = src[n & 3];
        dest[1]     = src[(n>>2) & 3];
        dest[2]     = src[(n>>4) & 3];
        dest[3]     = src[(n>>6) & 3];
        WriteOperand64(inst, inst->Main.Argument1, 0, res);
    }
}

void Processor::Stmxcsr_0FAE_3(const Instruction *inst)
{
    // STMXCSR
    u32 offset = Offset32(inst->Main.Argument1);
    u32 mxcsr = SIMD.MXCSR;
    WriteOperand32(inst, inst->Main.Argument1, offset, mxcsr);
    
}

void Processor::Pcmpistri_660F3A63(const Instruction *inst)
{
    // PCMPISTRI
    byte imm = (byte) inst->Main.Inst.Immediat;
    if (imm != 0x0d) {
        NOT_IMPLEMENTED();
    }
    u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
    u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
    __m128i a = *((__m128i *) &val1);
    __m128i b = *((__m128i *) &val2);
    ECX = _mm_cmpistri(a, b, 0x0d);
}

END_NAMESPACE_LOCHSEMU()