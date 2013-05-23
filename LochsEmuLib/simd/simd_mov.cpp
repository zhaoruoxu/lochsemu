#include "stdafx.h"
#include "processor.h"
#include "simd.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Movsd_F20F10(const Instruction *inst)
{
    // MOVSD xmm1, xmm2/m64
    u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
    u128 val2;
    if (IsMemoryArg(inst->Main.Argument2)) {
        u64 t2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
        memcpy(&val2, &t2, sizeof(u64));
    } else if (IsRegArg(inst->Main.Argument2)) {
        val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
    } else {
        Assert(0);
    }
    val1.dat[0] = val2.dat[0]; val1.dat[1] = val2.dat[1];
    WriteOperand128(inst, inst->Main.Argument1, 0, val1);
}

void Processor::Movsd_F20F11(const Instruction *inst)
{
    // MOVSD xmm2/m64, xmm1
    u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
    if (IsMemoryArg(inst->Main.Argument1)) {
        u64 val1;
        memcpy(&val1, &val2, sizeof(u64));
        WriteOperand64(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val1);
    } else if (IsRegArg(inst->Main.Argument1)) {
        u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
        val1.dat[0] = val2.dat[0]; val1.dat[1] = val2.dat[1];
        WriteOperand128(inst, inst->Main.Argument1, 0, val1);
    } else {
        Assert(0);
    }
}

void Processor::Movlpd_0F12(const Instruction *inst)
{
    // MOVLPD xmm, m64
    Assert(inst->Main.Prefix.OperandSize);

    u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
    u64 val2 = ReadOperand64(inst, inst->Main.Argument2, NULL);
    val1.qw[0] = val2;
    WriteOperand128(inst, inst->Main.Argument1, 0, val1);
}

void Processor::Movlpd_0F13(const Instruction *inst)
{
    // MOVLPD m64, xmm
    Assert(inst->Main.Prefix.OperandSize);

    u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
    WriteOperand64(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val2.qw[0]);
}

void Processor::Movapd_66_0F28(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // MOVAPD xmm1, xmm2/m128
        u128 val = ReadOperand128(inst, inst->Main.Argument2, NULL);
        WriteOperand128(inst, inst->Main.Argument1, 0, val);
    } else {
        NOT_IMPLEMENTED();
    }
    
}

void Processor::Movd_0F6E(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // MOVD xmm, r/m32
        u32 val = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u128 r;
        r.dat[0] = val;
        WriteOperand128(inst, inst->Main.Argument1, 0, r);
    } else {
        // MOVD mm, r/m32
        u32 val = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u64 r = val;
        WriteOperand64(inst, inst->Main.Argument1, 0, r);
    }
    
}

void Processor::Movdqa_0F6F(const Instruction *inst)
{
    if (inst->Main.Prefix.RepPrefix) {
        // return Movdqu_F30F6F(inst);
        // Same with movdqa
    }
    if (inst->Main.Prefix.OperandSize) {
        // MOVDQA
        u128 val = ReadOperand128(inst, inst->Main.Argument2, NULL);
        WriteOperand128(inst, inst->Main.Argument1, 0, val);
    } else {
        //NOT_IMPLEMENTED();
        u64 val = ReadOperand64(inst, inst->Main.Argument2, NULL);
        WriteOperand64(inst, inst->Main.Argument1, 0, val);
    }
    
}

// void Processor::Movdqu_F30F6F(const Instruction *inst)
// {
//     // MOVDQU xmm1, xmm2/m128
// }

void Processor::Movd_0F7E(const Instruction *inst)
{
    if (inst->Main.Prefix.RepPrefix) {
        // MOVQ F30F7E
        if (IsMemoryArg(inst->Main.Argument2)) {
            u64 r = ReadOperand64(inst, inst->Main.Argument2, NULL);
            u128 val;
            memcpy(&val, &r, sizeof(u64));
            WriteOperand128(inst, inst->Main.Argument1, 0, val);
        } else {
            Assert(IsRegArg(inst->Main.Argument1));
            u128 val1 = ReadOperand128(inst, inst->Main.Argument1, NULL);
            u128 val2 = ReadOperand128(inst, inst->Main.Argument2, NULL);
            val1.dat[0] = val2.dat[0];
            val1.dat[1] = val2.dat[1];
            WriteOperand128(inst, inst->Main.Argument1, 0, val1);
        } 
    } else if (inst->Main.Prefix.OperandSize) {
        // 66 prefix, sse
        u128 val = ReadOperand128(inst, inst->Main.Argument2, NULL);
        WriteOperand32(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1),
            val.dat[0]);
    } else {
        u64 val = ReadOperand64(inst, inst->Main.Argument2, NULL);
        WriteOperand32(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1),
            (u32) val);
    }
    
}

void Processor::Movdqa_0F7F(const Instruction *inst)
{
    if (inst->Main.Prefix.RepPrefix) {
        //return Movdqu_F30F7F(inst);
        // Same with movdqa
    }
    if (inst->Main.Prefix.OperandSize) {	//SSE
        // MOVDQA
        u128 val = ReadOperand128(inst, inst->Main.Argument2, NULL);
        WriteOperand128(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val);
    } else {												// MMX
        u64 val = ReadOperand64(inst, inst->Main.Argument2, NULL);
		WriteOperand64(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val);
    }
    
}

// void Processor::Movdqu_F30F7F(const Instruction *inst)
// {
//     NOT_IMPLEMENTED();
// }

void Processor::Movq_0FD6(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        // MOVQ xmm2/m64, xmm1
        u128 val = ReadOperand128(inst, inst->Main.Argument2, NULL);
        if (IsMemoryArg(inst->Main.Argument1)) {
            u64 r = reinterpret_cast<u64p>(&val)[0];
            WriteOperand64(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), r);
        } else {
            Assert(IsRegArg(inst->Main.Argument1));
            u128 r = ReadOperand128(inst, inst->Main.Argument1, NULL);
            r.dat[0] = val.dat[0];
            r.dat[1] = val.dat[1];
            r.dat[2] = r.dat[3] = 0;
            WriteOperand128(inst, inst->Main.Argument1, 0, r);
        }
    } else {
        LxFatal("shit happens\n");
    }
    
}

END_NAMESPACE_LOCHSEMU()