#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Cmp_38(const Instruction *inst)
{
    /**
     * CMP r/m8, r8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    
}

void Processor::Cmp_39(const Instruction *inst)
{
    /**
     * CMP r/m16, r16
     * CMP r/m32, r32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 r = val1 - val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 - val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
    }
    
}

void Processor::Cmp_3A(const Instruction *inst)
{
    /**
     * CMP r8, r/m8
     */
    if (EIP == 0x7854fe7e) {
        DebugBreak();
    }
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    
}

void Processor::Cmp_3B(const Instruction *inst)
{
    /**
     * CMP r16, r/m16
     * CMP r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 r = val1 - val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 - val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
    }
    
}

void Processor::Cmp_3C(const Instruction *inst)
{
    /**
     * CMP AL, imm8
     */
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = AL - val2;
    SetFlagsArith8(r, PROMOTE_U16(AL) - PROMOTE_U16(val2),
        PROMOTE_I16(AL) - PROMOTE_I16(val2));
    
}

void Processor::Cmp_3D(const Instruction *inst)
{
    /**
     * CMP AX, imm16
     * CMP EAX, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        u16 r = AX - val2;
        SetFlagsArith16(r, PROMOTE_U32(AX) - PROMOTE_U32(val2),
            PROMOTE_I32(AX) - PROMOTE_I32(val2));
    } else {
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 r = EAX - val2;
        SetFlagsArith32(r, PROMOTE_U64(EAX) - PROMOTE_U64(val2),
            PROMOTE_I64(EAX) - PROMOTE_I64(val2));
    }
    
}

void Processor::Cmp_80_ext7(const Instruction *inst)
{
    /**
     * CMP r/m8, imm8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = val1 - val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    
}

void Processor::Cmp_81_ext7(const Instruction *inst)
{
    /**
     * CMP r/m16, imm16
     * CMP r/m32, imm32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        u16 r = val1 - val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 r = val1 - val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
    }
    
}

void Processor::Cmp_83_ext7(const Instruction *inst)
{
    /**
     * CMP r/m16, imm8
     * CMP r/m32, imm8
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = SIGN_EXTEND(8, 16, inst->Main.Inst.Immediat);
        u16 r = val1 - val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);
        u32 r = val1 - val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
    }
    
}

END_NAMESPACE_LOCHSEMU()