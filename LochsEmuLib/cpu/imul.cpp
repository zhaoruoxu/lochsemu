#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Imul_69(const Instruction *inst)
{
    /*
     * IMUL r16, r/m16, imm16
     * IMUL r32, r/m32, imm32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
    u32 val3 = (u32) inst->Main.Inst.Immediat;
    i64 r64 = PROMOTE_I64(val2) * PROMOTE_I64(val3);
    i32 r32 = (i32) val2 * (i32) val3;
    WriteOperand32(inst, inst->Main.Argument1, 0, (u32) r32);
    if (r64 != (i64) r32) {
        CF = OF = 1;
    } else {
        CF = OF = 0;
    }
    
}

void Processor::Imul_6B(const Instruction *inst)
{
    /**
     * IMUL r32, r/m32, imm8
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
    u32 val3 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);
    i64 r64 = PROMOTE_I64(val2) * PROMOTE_I64(val3);
    i32 r32 = (i32) val2 * (i32) val3;
    WriteOperand32(inst, inst->Main.Argument1, 0, (u32) r32);
    if (r64 != (i64) r32) {
        CF = OF = 1;
    } else {
        CF = OF = 0;
    }
    
}

void Processor::Imul_F6_ext5(const Instruction *inst)
{
    // IMUL r/m8
    u8 val1 = AL;
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    i16 r = PROMOTE_I16(val1) * PROMOTE_I16(val2);
    AX = (u16) r;
    if (AH == 0) {
        CF = OF = 0;
    } else {
        CF = OF = 1;
    }
    
}

void Processor::Imul_F7_ext5(const Instruction *inst)
{
    // IMUL r/m32
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 val1 = EAX;
    u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
    i64 r = PROMOTE_I64(val1) * PROMOTE_I64(val2);
    EAX = (u32) r;
    EDX = (u32) (r >> 32);
    if (EDX == 0) {
        CF = OF = 0;
    } else {
        CF = OF = 1;
    }
    
}

void Processor::Imul_0FAF(const Instruction *inst)
{
    /**
     * IMUL r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
    u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
    i64 r64 = PROMOTE_I64(val1) * PROMOTE_I64(val2);
    i32 r32 = (i32) val1 * (i32) val2;
    WriteOperand32(inst, inst->Main.Argument1, 0, (u32) r32);
    if (r64 != (i64) r32) {
        CF = OF = 1;
    } else {
        CF = OF = 0;
    }
    
}

END_NAMESPACE_LOCHSEMU()