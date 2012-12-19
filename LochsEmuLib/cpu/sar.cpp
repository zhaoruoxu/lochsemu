#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define COUNT_MASK      0x1f

static INLINE void Sar32(Processor *cpu, u32 &a, u8 b)
{
    i32 r = (i32) a;
    b &= COUNT_MASK;
    if (b > 0) {
        cpu->CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            cpu->OF = 0;
        }
        a = (u32) r;
        cpu->SetFlagsShift32(r);
    }
}

static INLINE void Sar8(Processor *cpu, u8 &a, u8 b)
{
    i8 r = (i8) a;
    b &= COUNT_MASK;
    if (b > 0) {
        cpu->CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            cpu->OF = 0;
        }
        a = (u8) r;
        cpu->SetFlagsShift8(r);
    }
}

LxResult Sar_C0_ext7(Processor *cpu, const Instruction *inst)
{
    /**
     * SAR r/m8, imm8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Sar8(cpu, val1, val2);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Sar_C1_ext7(Processor *cpu, const Instruction *inst)
{
    /**
     * SAR r/m16, imm8
     * SAR r/m32, imm8
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 offset;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Sar32(cpu, val1, val2);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

LxResult Sar_D0_ext7(Processor *cpu, const Instruction *inst)
{
    /**
     * SAR r/m8, 1
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    Sar8(cpu, val1, 1);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Sar_D1_ext7(Processor *cpu, const Instruction *inst)
{
    /**
     * SAR r/m16, 1
     * SAR r/m32, 1
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 offset;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        Sar32(cpu, val1, 1);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

LxResult Sar_D2_ext7(Processor *cpu, const Instruction *inst)
{
    /**
     * SAR r/m8, CL
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    Sar8(cpu, val1, cpu->CL);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Sar_D3_ext7(Processor *cpu, const Instruction *inst)
{
    /**
     * SAR r/m16, CL
     * SAR r/m32, CL
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 offset;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        Sar32(cpu, val1, cpu->CL);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()