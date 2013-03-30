#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::And_20(const Instruction *inst)
{
    /**
     * AND r/m8, r8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 & val2;
    SetFlagsLogic8(r);
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::And_21(const Instruction *inst)
{
    /**
     * AND r/m16, r16
     * AND r/m32, r32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 r = val1 & val2;
		SetFlagsLogic16(r);
		WriteOperand16(inst, inst->Main.Argument1, offset, r);    
	} else {
        u32 offset;
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 & val2;
        SetFlagsLogic32(r);
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

void Processor::And_22(const Instruction *inst)
{
    /**
     * AND r8, r/m8
     */
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 & val2;
    SetFlagsLogic8(r);
    WriteOperand8(inst, inst->Main.Argument1, 0, r);
    
}

void Processor::And_23(const Instruction *inst)
{
    /**
     * AND r16, r/m16
     * AND r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, NULL);
		u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 r = val1 & val2;
		SetFlagsLogic16(r);
		WriteOperand16(inst, inst->Main.Argument1, 0, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 & val2;
        SetFlagsLogic32(r);
        WriteOperand32(inst, inst->Main.Argument1, 0, r);
    }
    
}

void Processor::And_24(const Instruction *inst)
{
    // AND AL, imm8
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = AL & val2;
    SetFlagsLogic8(r);
    AL = r;
    
}

void Processor::And_25(const Instruction *inst)
{
    /**
     * AND EAX, r/m32
     */
	if (inst->Main.Prefix.OperandSize){
		u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 r = AX & val2;
		SetFlagsLogic16(r);
		AX = r;
	} else {
		u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
		u32 r = EAX & val2;
		SetFlagsLogic32(r);
		EAX = r;
	}
    
}

void Processor::And_80_ext4(const Instruction *inst)
{
    /**
     * AND r/m8, imm8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = val1 & val2;
    SetFlagsLogic8(r);
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::And_81_ext4(const Instruction *inst)
{
    /**
     * AND r/m16, imm16
     * AND r/m32, imm32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        u16 r = val1 & val2;
        SetFlagsLogic16(r);
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 r = val1 & val2;
        SetFlagsLogic32(r);
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

void Processor::And_83_ext4(const Instruction *inst)
{
    /**
     * AND r/m16, imm8
     * AND r/m32, imm8
     */
	u32 offset;
    if (inst->Main.Prefix.OperandSize) {
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = SIGN_EXTEND(8, 16, inst->Main.Inst.Immediat);
		u16 r = val1 & val2;
		SetFlagsLogic16(r);
		WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 offset;
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);
        u32 r = val1 & val2;
        SetFlagsLogic32(r);
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

END_NAMESPACE_LOCHSEMU()