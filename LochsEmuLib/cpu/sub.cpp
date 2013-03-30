#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Sub_28(const Instruction *inst)
{
    /**
     * SUB r/m8, r8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::Sub_29(const Instruction *inst)
{
    /**
     * SUB r/m16, r16
     * SUB r/m32, r32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 r = val1 - val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 - val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

void Processor::Sub_2A(const Instruction *inst)
{
    /**
     * SUB r8, r/m8
     */
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    WriteOperand8(inst, inst->Main.Argument1, 0, r);
    
}

void Processor::Sub_2B(const Instruction *inst)
{
    /**
     * SUB r16, r/m16
     * SUB r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 r = val1 - val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
        WriteOperand16(inst, inst->Main.Argument1, 0, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 - val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
        WriteOperand32(inst, inst->Main.Argument1, 0, r);
    }
    
}

void Processor::Sub_2C(const Instruction *inst)
{
    /**
     * SUB AL, imm8
     */
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = AL - val2;
    SetFlagsArith8(r, PROMOTE_U32(AL) - PROMOTE_U32(val2),
        PROMOTE_I32(AL) - PROMOTE_I32(val2));
    AL = r;
    
}

void Processor::Sub_2D(const Instruction *inst)
{
    /**
     * SUB EAX, imm32
     * SUB AX, imm16
     */
    if (inst->Main.Prefix.OperandSize) {
        NOT_IMPLEMENTED();
    } else {
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 r = EAX - val2;
        SetFlagsArith32(r, PROMOTE_U64(EAX) - PROMOTE_U64(val2),
            PROMOTE_I64(EAX) - PROMOTE_I64(val2));
        EAX = r;
    }
    
}

void Processor::Sub_80_ext5(const Instruction *inst)
{
    /**
     * SUB r/m8, imm8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::Sub_81_ext5(const Instruction *inst)
{
    /**
     * SUB r/m16, imm16
     * SUB r/m32, imm32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, &offset);
        u16 r = val1 - val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, &offset);
        u32 r = val1 - val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

void Processor::Sub_83_ext5(const Instruction *inst)
{
    /**
     * SUB r/m16, imm8 SE
     * SUB r/m32, imm8 SE
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = SIGN_EXTEND(8, 16, inst->Main.Inst.Immediat);
        u16 r = val1 - val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);
        u32 r = val1 - val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}


END_NAMESPACE_LOCHSEMU()