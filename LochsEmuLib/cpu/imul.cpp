#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Imul_69(Processor *cpu, const Instruction *inst)
{
    /*
     * IMUL r16, r/m16, imm16
     * IMUL r32, r/m32, imm32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    u32 val3 = (u32) inst->Main.Inst.Immediat;
    i64 r64 = PROMOTE_I64(val2) * PROMOTE_I64(val3);
    i32 r32 = (i32) val2 * (i32) val3;
    cpu->WriteOperand32(inst, inst->Main.Argument1, 0, (u32) r32);
    if (r64 != (i64) r32) {
        cpu->CF = cpu->OF = 1;
    } else {
        cpu->CF = cpu->OF = 0;
    }
    RET_SUCCESS();
}

LxResult Imul_6B(Processor *cpu, const Instruction *inst)
{
    /**
     * IMUL r32, r/m32, imm8
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    u32 val3 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);
    i64 r64 = PROMOTE_I64(val2) * PROMOTE_I64(val3);
    i32 r32 = (i32) val2 * (i32) val3;
    cpu->WriteOperand32(inst, inst->Main.Argument1, 0, (u32) r32);
    if (r64 != (i64) r32) {
        cpu->CF = cpu->OF = 1;
    } else {
        cpu->CF = cpu->OF = 0;
    }
    RET_SUCCESS();
}

LxResult Imul_F6_ext5(Processor *cpu, const Instruction *inst)
{
    // IMUL r/m8
    u8 val1 = cpu->AL;
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    i16 r = PROMOTE_I16(val1) * PROMOTE_I16(val2);
    cpu->AX = (u16) r;
    if (cpu->AH == 0) {
        cpu->CF = cpu->OF = 0;
    } else {
        cpu->CF = cpu->OF = 1;
    }
    RET_SUCCESS();
}

LxResult Imul_F7_ext5(Processor *cpu, const Instruction *inst)
{
    // IMUL r/m32
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 val1 = cpu->EAX;
    u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    i64 r = PROMOTE_I64(val1) * PROMOTE_I64(val2);
    cpu->EAX = (u32) r;
    cpu->EDX = (u32) (r >> 32);
    if (cpu->EDX == 0) {
        cpu->CF = cpu->OF = 0;
    } else {
        cpu->CF = cpu->OF = 1;
    }
    RET_SUCCESS();
}

LxResult Imul_0FAF(Processor *cpu, const Instruction *inst)
{
    /**
     * IMUL r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
    u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    i64 r64 = PROMOTE_I64(val1) * PROMOTE_I64(val2);
    i32 r32 = (i32) val1 * (i32) val2;
    cpu->WriteOperand32(inst, inst->Main.Argument1, 0, (u32) r32);
    if (r64 != (i64) r32) {
        cpu->CF = cpu->OF = 1;
    } else {
        cpu->CF = cpu->OF = 0;
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()