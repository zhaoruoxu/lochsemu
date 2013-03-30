#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define COUNT_MASK      0x1f

void Processor::Shl8(u8 &a, u8 b)
{
    u8 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (8-b))) ? 1 : 0;
        r <<= b;
        if (b == 1) {
            OF = MSB8(r) ^ CF;
        }
        a = r;
        SetFlagsShift8(r);
    }
}

void Processor::Shl16(u16 &a, u8 b)
{
    u16 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (16-b))) ? 1 : 0;
        r <<= b;
        if (b == 1) {
            OF = MSB16(r) ^ CF;
        }
        a = r;
        SetFlagsShift16(r);
    }
}

void Processor::Shl32(u32 &a, u8 b)
{
    u32 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (32-b))) ? 1 : 0;
        r <<= b;
        if (b == 1) {
            OF = MSB32(r) ^ CF;
        }
        a = r;
        SetFlagsShift32(r);
    }
}

void Processor::Shl_C0_ext4(const Instruction *inst)
{
    /**
     * SHL r/m8, imm8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Shl8(val1, val2);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Shl_C1_ext4(const Instruction *inst)
{
    /**
     * SHL r/m16, imm8
     * SHl r/m32, imm8
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Shl16(val1, val2);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Shl32(val1, val2);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

void Processor::Shl_D0_ext4(const Instruction *inst)
{
    /**
     * SHL r/m8, 1
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    Shl8(val1, 1);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Shl_D1_ext4(const Instruction *inst)
{
    /**
     * SHL r/m16, 1
     * SHl r/m32, 1
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        Shl16(val1, 1);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Shl32(val1, 1);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

void Processor::Shl_D2_ext4(const Instruction *inst)
{
    /**
     * SHL r/m8, CL
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    Shl8(val1, CL);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Shl_D3_ext4(const Instruction *inst)
{
    /**
     * SHL r/m16/32, CL
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        Shl16(val1, CL);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Shl32(val1, CL);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

void Processor::Shld_0FA4(const Instruction *inst)
{
    // SHLD r/m32, r32, imm8
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    u32 offset;
    u32 val1    = ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 val2    = ReadOperand32(inst, inst->Main.Argument2, NULL);
    u8 cnt      = (u8) inst->Main.Inst.Immediat;

    cnt %= 0x1f;
    if (cnt == 0) 
    Assert(cnt < 32);
    CF     = (val1 & (1 << (32-cnt))) ? 1 : 0;

    u32 r;
    __asm {
        mov     eax, val1
        mov     edx, val2
        mov     cl, cnt
        shld    eax, edx, cl
        mov     r, eax
    }

    SetFlagsShift32(r);
    if (cnt == 1) {
        OF = MSB32(val1) == MSB32(r) ? 0 : 1;
    }
    WriteOperand32(inst, inst->Main.Argument1, offset, r);
    

}

void Processor::Shld_0FA5(const Instruction *inst)
{
    // SHLD r/m32, r32, imm8
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    u32 offset;
    u32 val1    = ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 val2    = ReadOperand32(inst, inst->Main.Argument2, NULL);
    u8 cnt      = CF;

    cnt %= 0x1f;
    if (cnt == 0) 
    Assert(cnt < 32);
    CF     = (val1 & (1 << (32-cnt))) ? 1 : 0;

    u32 r;
    __asm {
        mov     eax, val1
        mov     edx, val2
        mov     cl, cnt
        shld    eax, edx, cl
        mov     r, eax
    }

    SetFlagsShift32(r);
    if (cnt == 1) {
        OF = MSB32(val1) == MSB32(r) ? 0 : 1;
    }
    WriteOperand32(inst, inst->Main.Argument1, offset, r);
    
}

END_NAMESPACE_LOCHSEMU()