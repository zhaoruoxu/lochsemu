#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define COUNT_MASK      0x1f

static INLINE void Shl8(Processor *cpu, u8 &a, u8 b)
{
    u8 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        cpu->CF = (r & (1 << (8-b))) ? 1 : 0;
        r <<= b;
        if (b == 1) {
            cpu->OF = MSB8(r) ^ cpu->CF;
        }
        a = r;
        cpu->SetFlagsShift8(r);
    }
}

static INLINE void Shl16(Processor *cpu, u16 &a, u8 b)
{
    u16 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        cpu->CF = (r & (1 << (16-b))) ? 1 : 0;
        r <<= b;
        if (b == 1) {
            cpu->OF = MSB16(r) ^ cpu->CF;
        }
        a = r;
        cpu->SetFlagsShift16(r);
    }
}

static INLINE void Shl32(Processor *cpu, u32 &a, u8 b)
{
    u32 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        cpu->CF = (r & (1 << (32-b))) ? 1 : 0;
        r <<= b;
        if (b == 1) {
            cpu->OF = MSB32(r) ^ cpu->CF;
        }
        a = r;
        cpu->SetFlagsShift32(r);
    }
}

LxResult Shl_C0_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * SHL r/m8, imm8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Shl8(cpu, val1, val2);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Shl_C1_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * SHL r/m16, imm8
     * SHl r/m32, imm8
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Shl16(cpu, val1, val2);
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Shl32(cpu, val1, val2);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

LxResult Shl_D0_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * SHL r/m8, 1
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    Shl8(cpu, val1, 1);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Shl_D1_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * SHL r/m16, 1
     * SHl r/m32, 1
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        Shl16(cpu, val1, 1);
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        Shl32(cpu, val1, 1);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

LxResult Shl_D2_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * SHL r/m8, CL
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    Shl8(cpu, val1, cpu->CL);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Shl_D3_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * SHL r/m16/32, CL
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        Shl16(cpu, val1, cpu->CL);
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        Shl32(cpu, val1, cpu->CL);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

LxResult Shld_0FA4(Processor *cpu, const Instruction *inst)
{
    // SHLD r/m32, r32, imm8
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    u32 offset;
    u32 val1    = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 val2    = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    u8 cnt      = (u8) inst->Main.Inst.Immediat;

    cnt %= 0x1f;
    if (cnt == 0) RET_SUCCESS();
    Assert(cnt < 32);
    cpu->CF     = (val1 & (1 << (32-cnt))) ? 1 : 0;

    u32 r;
    __asm {
        mov     eax, val1
        mov     edx, val2
        mov     cl, cnt
        shld    eax, edx, cl
        mov     r, eax
    }

    cpu->SetFlagsShift32(r);
    if (cnt == 1) {
        cpu->OF = MSB32(val1) == MSB32(r) ? 0 : 1;
    }
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();

}

LxResult Shld_0FA5(Processor *cpu, const Instruction *inst)
{
    // SHLD r/m32, r32, imm8
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    u32 offset;
    u32 val1    = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 val2    = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    u8 cnt      = cpu->CF;

    cnt %= 0x1f;
    if (cnt == 0) RET_SUCCESS();
    Assert(cnt < 32);
    cpu->CF     = (val1 & (1 << (32-cnt))) ? 1 : 0;

    u32 r;
    __asm {
        mov     eax, val1
        mov     edx, val2
        mov     cl, cnt
        shld    eax, edx, cl
        mov     r, eax
    }

    cpu->SetFlagsShift32(r);
    if (cnt == 1) {
        cpu->OF = MSB32(val1) == MSB32(r) ? 0 : 1;
    }
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()