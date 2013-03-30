#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Or_08(const Instruction *inst)
{
    /**
     * OR r/m8, r8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 | val2;
    SetFlagsLogic8(r);
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::Or_09(const Instruction *inst)
{
    /**
     * OR r/m32, r32
     * OR r/m16, r16
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);

        u16 r = val1 | val2;
        SetFlagsLogic16(r);
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);

        u32 r = val1 | val2;
        SetFlagsLogic32(r);
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

void Processor::Or_0A(const Instruction *inst)
{
    /**
     * OR r8, r/m8
     */
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 | val2;
    SetFlagsLogic8(r);
    WriteOperand8(inst, inst->Main.Argument1, 0, r);
    
}

void Processor::Or_0B(const Instruction *inst)
{
    /**
     * OR r16, r/m16
     * OR r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);

        u16 r = val1 | val2;
        SetFlagsLogic16(r);
        WriteOperand16(inst, inst->Main.Argument1, 0, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);

        u32 r = val1 | val2;
        SetFlagsLogic32(r);
        WriteOperand32(inst, inst->Main.Argument1, 0, r);
    }
    
}

void Processor::Or_0C(const Instruction *inst)
{
	//OR AL, imm8
	u8 r = AL | (u8) inst->Main.Inst.Immediat;
	SetFlagsLogic8(r);
	AL=r;
	
}

void Processor::Or_0D(const Instruction *inst)
{
    /**
     * OR EAX, imm32
     */
	if (inst->Main.Prefix.OperandSize){
		u16 r=AX | (u16) inst->Main.Inst.Immediat;
		SetFlagsLogic16(r);
		AX=r;
	}
	else{
		u32 r = EAX | (u32) inst->Main.Inst.Immediat;
		SetFlagsLogic32(r);
		EAX = r;
	}
    
}

void Processor::Or_80_ext1(const Instruction *inst)
{
    /**
     * OR r/m8, imm8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;

    u8 r = val1 | val2;
    SetFlagsLogic8(r);
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::Or_81_ext1(const Instruction *inst)
{
    /**
     * OR r/m32, imm32
     */
	u32 offset;
	if (inst->Main.Prefix.OperandSize) {
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = (u16) inst->Main.Inst.Immediat;

		u16 r = val1 | val2;
		SetFlagsLogic16(r);
		WriteOperand16(inst, inst->Main.Argument1, offset, r);
	} else {
		u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = (u32) inst->Main.Inst.Immediat;

		u32 r = val1 | val2;
		SetFlagsLogic32(r);
		WriteOperand32(inst, inst->Main.Argument1, offset, r);		
	}
	
}

void Processor::Or_83_ext1(const Instruction *inst)
{
    /**
     * OR r/m32, imm8
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument1, &offset);

        u16 r = val1 | val2;
        SetFlagsLogic16(r);
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);

        u32 r = val1 | val2;
        SetFlagsLogic32(r);
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

END_NAMESPACE_LOCHSEMU()