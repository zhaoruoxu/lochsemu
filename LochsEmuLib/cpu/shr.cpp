#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define COUNT_MASK      0x1f

void Processor::Shr32(u32 &a, u8 b)
{
    u32 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            OF = (a & 0x80000000) ? 1 : 0;
        }
        a = r;
        SetFlagsShift32(r);
    }
}

void Processor::Shr8(u8 &a, u8 b)
{
    u8 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            OF = (a & 0x80) ? 1 : 0;
        }
        a = r;
        SetFlagsShift8(r);
    }
}

void Processor::Shr16(u16 &a, u8 b)
{
    u16 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            OF = (a & 0x8000) ? 1 : 0;
        }
        a = r;
        SetFlagsShift16(r);
    }
}

void Processor::Shr_C0_ext5(const Instruction *inst)
{
    /**
     * SHR r/m8, imm8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Shr8(val1, val2);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Shr_C1_ext5(const Instruction *inst)
{
    /**
     * SHR r/m16, imm8
     * SHR r/m32, imm8
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Shr16(val1, val2);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Shr32(val1, val2);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

void Processor::Shr_D0_ext5(const Instruction *inst)
{
    /**
     * SHR r/m8, 1
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    Shr8(val1, 1);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Shr_D1_ext5(const Instruction *inst)
{
    /**
     * SHR r/m16, 1
     * SHR r/m32, 1
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        Shr16(val1, 1);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Shr32(val1, 1);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

void Processor::Shr_D2_ext5(const Instruction *inst)
{
    /**
     * SHR r/m8, CL
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    Shr8(val1, CL);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Shr_D3_ext5(const Instruction *inst)
{
    /**
     * SHR r/m16, CL
     * SHR r/m32, CL
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        Shr16(val1, CL);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Shr32(val1, CL);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

void Processor::Shrd_0FAC(const Instruction *inst)
{
    // SHRD r/m32, r32, imm8
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    u32 offset;
    u32 val1    = ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 val2    = ReadOperand32(inst, inst->Main.Argument2, NULL);
    u8 cnt      = (u8) inst->Main.Inst.Immediat;
    
    cnt %= 0x1f;
    if (cnt == 0) 
    Assert(cnt < 32);
    CF     = (val1 >> (cnt-1)) & 0x1;

    u32 r;
    __asm {
        mov     eax, val1
        mov     edx, val2
        mov     cl, cnt
        shrd    eax, edx, cl
        mov     r, eax
    }

    SetFlagsShift32(r);
    if (cnt == 1) {
        OF = MSB32(val1) == MSB32(r) ? 0 : 1;
    }
    WriteOperand32(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::Shrd_0FAD(const Instruction *inst)
{
    // SHRD r/m32, r32, imm8
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    u32 offset;
    u32 val1    = ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 val2    = ReadOperand32(inst, inst->Main.Argument2, NULL);
    u8 cnt      = CL;

    cnt %= 0x1f;
    if (cnt == 0) 
    Assert(cnt < 32);
    CF     = (val1 >> (cnt-1)) & 0x1;

    u32 r;
    __asm {
        mov     eax, val1
        mov     edx, val2
        mov     cl, cnt
        shrd    eax, edx, cl
        mov     r, eax
    }

    SetFlagsShift32(r);
    if (cnt == 1) {
        OF = MSB32(val1) == MSB32(r) ? 0 : 1;
    }
    WriteOperand32(inst, inst->Main.Argument1, offset, r);
    
}

END_NAMESPACE_LOCHSEMU()