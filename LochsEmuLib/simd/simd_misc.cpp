#include "stdafx.h"
#include "processor.h"
#include "simd.h"
#include <mmintrin.h>

BEGIN_NAMESPACE_LOCHSEMU()

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

void Processor::Stmxcsr_0FAE_3(const Instruction *inst)
{
    // STMXCSR
    u32 offset = Offset32(inst->Main.Argument1);
    u32 mxcsr = SIMD.MXCSR;
    WriteOperand32(inst, inst->Main.Argument1, offset, mxcsr);
    
}

END_NAMESPACE_LOCHSEMU()