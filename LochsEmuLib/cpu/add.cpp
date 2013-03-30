#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Add_00(const Instruction *inst)
{
    /**
     * ADD r/m8, r8
     */
    u32 offset = 0;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    
    u8 r = val1 + val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2), PROMOTE_I16(val1) + PROMOTE_I16(val2));
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
}

void Processor::Add_01(const Instruction *inst)
{
    /**
     * ADD r/m16/32 r16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        // 16-bit operands
        u32 offset = 0;
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        
        u16 r = val1 + val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2), PROMOTE_I32(val1) + PROMOTE_I32(val2));
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        // 32-bit operands
        u32 offset = 0;
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, &offset);
        
        u32 r = val1 + val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2), PROMOTE_I64(val1) + PROMOTE_I64(val2));
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
}

void Processor::Add_02(const Instruction *inst)
{
    /**
     * ADD r8, r/m8
     */
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 + val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2),
        PROMOTE_I16(val1) + PROMOTE_I16(val2));
    WriteOperand8(inst, inst->Main.Argument1, 0, r);
}

void Processor::Add_03(const Instruction *inst)
{
    /**
     * ADD r16/32, r/m16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        NOT_IMPLEMENTED();
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 + val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2),
            PROMOTE_I64(val1) + PROMOTE_I64(val2));
        WriteOperand32(inst, inst->Main.Argument1, 0, r);
    }
}

void Processor::Add_04(const Instruction *inst)
{
    // ADD AL, imm8
    u8 val1 = AL;
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = val1 + val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2),
        PROMOTE_I16(val1) + PROMOTE_I16(val2));
    AL = r;
}

void Processor::Add_05(const Instruction *inst)
{
    /**
     * ADD EAX, imm32
     */
	if (inst->Main.Prefix.OperandSize){
		u16 val1=AX;
		u16 val2=(u16)inst->Main.Inst.Immediat;
		u16 r=val1+val2;
		SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2), PROMOTE_I32(val1) + PROMOTE_I32(val2));
		AX=r;
	}
	else{
		u32 val1 = EAX;
		u32 val2 = (u32) inst->Main.Inst.Immediat;
		u32 r = val1 + val2;
		SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2),
			PROMOTE_I64(val1) + PROMOTE_I64(val2));
		EAX = r;
	}
}

void Processor::Add_80_ext0(const Instruction *inst)
{
    /**
     * ADD r/m8, imm8
     */
    u32 offset = 0;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;

    u8 r = val1 + val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2),
        PROMOTE_I16(val1) + PROMOTE_I16(val2));
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
}

void Processor::Add_81_ext0(const Instruction *inst)
{
    /**
     * ADD r/m16, imm16
     * ADD r/m32, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        u32 offset = 0;
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = (u16) inst->Main.Inst.Immediat;

        u16 r = val1 + val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2),
            PROMOTE_I32(val1) + PROMOTE_I32(val2));
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 offset = 0;
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = (u32) inst->Main.Inst.Immediat;

        u32 r = val1 + val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2),
            PROMOTE_I64(val1) + PROMOTE_I64(val2));
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
}   

void Processor::Add_83_ext0(const Instruction *inst)
{
    /**
     * ADD r/m16, imm8
     * ADD r/m32, imm8
     */
    if (inst->Main.Prefix.OperandSize) {
        u32 offset = 0;
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = SIGN_EXTEND(8, 16, inst->Main.Inst.Immediat);

        u16 r = val1 + val2;
        SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2),
            PROMOTE_I32(val1) + PROMOTE_I32(val2));
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 offset = 0;
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);

        u32 r = val1 + val2;
        SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2),
            PROMOTE_I64(val1) + PROMOTE_I64(val2));
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
}

END_NAMESPACE_LOCHSEMU()