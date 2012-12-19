#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define COUNT_MASK      0x1f

static INLINE void Shr32(Processor *cpu, u32 &a, u8 b)
{
    u32 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        cpu->CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            cpu->OF = (a & 0x80000000) ? 1 : 0;
        }
        a = r;
        cpu->SetFlagsShift32(r);
    }
}

static INLINE void Shr8(Processor *cpu, u8 &a, u8 b)
{
    u8 r = a;
    b &= COUNT_MASK;
    if (b > 0) {
        cpu->CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            cpu->OF = (a & 0x80) ? 1 : 0;
        }
        a = r;
        cpu->SetFlagsShift8(r);
    }
}

LxResult Shr_C0_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SHR r/m8, imm8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Shr8(cpu, val1, val2);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Shr_C1_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SHR r/m16, imm8
     * SHR r/m32, imm8
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 offset;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Shr32(cpu, val1, val2);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

LxResult Shr_D0_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SHR r/m8, 1
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    Shr8(cpu, val1, 1);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Shr_D1_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SHR r/m16, 1
     * SHR r/m32, 1
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 offset;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        Shr32(cpu, val1, 1);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

LxResult Shr_D2_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SHR r/m8, CL
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    Shr8(cpu, val1, cpu->CL);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Shr_D3_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SHR r/m16, CL
     * SHR r/m32, CL
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        Shr32(cpu, val1, cpu->CL);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

LxResult Shrd_0FAC(Processor *cpu, const Instruction *inst)
{
    // SHRD r/m32, r32, imm8
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    u32 offset;
    u32 val1    = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 val2    = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    u8 cnt      = (u8) inst->Main.Inst.Immediat;
    
    cnt %= 0x1f;
    if (cnt == 0) RET_SUCCESS();
    Assert(cnt < 32);
    cpu->CF     = (val1 >> (cnt-1)) & 0x1;

    u32 r;
    __asm {
        mov     eax, val1
        mov     edx, val2
        mov     cl, cnt
        shrd    eax, edx, cl
        mov     r, eax
    }

    cpu->SetFlagsShift32(r);
    if (cnt == 1) {
        cpu->OF = MSB32(val1) == MSB32(r) ? 0 : 1;
    }
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Shrd_0FAD(Processor *cpu, const Instruction *inst)
{
    // SHRD r/m32, r32, imm8
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    u32 offset;
    u32 val1    = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 val2    = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    u8 cnt      = cpu->CL;

    cnt %= 0x1f;
    if (cnt == 0) RET_SUCCESS();
    Assert(cnt < 32);
    cpu->CF     = (val1 >> (cnt-1)) & 0x1;

    u32 r;
    __asm {
        mov     eax, val1
        mov     edx, val2
        mov     cl, cnt
        shrd    eax, edx, cl
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