#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Adc_11(const Instruction *inst)
{
    /**
     * ADC r/m32, r32
     */
    u32 offset;
	if (inst->Main.Prefix.OperandSize){
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 r = val1 + val2 + CF;
		SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2) + PROMOTE_U32(CF),
			PROMOTE_I32(val1) + PROMOTE_I32(val2) + PROMOTE_I32(CF));
		WriteOperand16(inst, inst->Main.Argument1, offset, r);

	} else {
		u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
		u32 r = val1 + val2 + CF;
		SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2) + PROMOTE_U64(CF),
			PROMOTE_I64(val1) + PROMOTE_I64(val2) + PROMOTE_I64(CF));
		WriteOperand32(inst, inst->Main.Argument1, offset, r);
	}
}

void Processor::Adc_12(const Instruction *inst)
{
    // ADC r8, r/m8
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 + val2 + CF;
    SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2) + PROMOTE_U16(CF),
        PROMOTE_I16(val1) + PROMOTE_I16(val2) + PROMOTE_I16(CF));
    WriteOperand8(inst, inst->Main.Argument1, 0, r);
}

void Processor::Adc_13(const Instruction *inst)
{
    /**
     * ADC r32, r/m32
     */
	if (inst->Main.Prefix.OperandSize){
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, NULL);
		u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 r = val1 + val2 + CF;
		SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2) + PROMOTE_U32(CF),
			PROMOTE_I32(val1) + PROMOTE_I32(val2) + PROMOTE_I32(CF));
		WriteOperand16(inst, inst->Main.Argument1, 0, r);
	} else {
		u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
		u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
		u32 r = val1 + val2 + CF;
		SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2) + PROMOTE_U64(CF),
			PROMOTE_I64(val1) + PROMOTE_I64(val2) + PROMOTE_I64(CF));
		WriteOperand32(inst, inst->Main.Argument1, 0, r);
	}
}

void Processor::Adc_14(const Instruction *inst)
{
    /*
     * ADC al, imm8
     */
    u8 val1 = AL;
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = val1 + val2 + CF;
    SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2) + PROMOTE_U16(CF),
        PROMOTE_I16(val1) + PROMOTE_I16(val2) + PROMOTE_I16(CF));
    AL = r;
}

void Processor::Adc_15(const Instruction *inst)
{
    /*
     * ADC ax/eax, imm16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = AX;
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        u16 r = val1 + val2 + CF;
        SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2) + PROMOTE_U32(CF),
            PROMOTE_I32(val1) + PROMOTE_I32(val2) + PROMOTE_I32(CF));
        AX = r;
    } else {
        u32 val1 = EAX;
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 r = val1 + val2 + CF;
        SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2) + PROMOTE_U64(CF),
            PROMOTE_I64(val1) + PROMOTE_I64(val2) + PROMOTE_I64(CF));
        EAX = r;
    }
}

void Processor::Adc_81_ext2(const Instruction *inst)
{
	// ADC r/m16/32, imm16/32
	u32 offset;
	if(inst->Main.Prefix.OperandSize){
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = (u16) inst->Main.Inst.Immediat;
		u16 r = val1 + val2 + CF;
		SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2) + PROMOTE_U32(CF),
			PROMOTE_I32(val1) + PROMOTE_I32(val2) + PROMOTE_I32(CF));
		WriteOperand16(inst, inst->Main.Argument1, offset, r);
	} else {
		u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = (u16) inst->Main.Inst.Immediat;
		u32 r = val1 + val2 + CF;
		SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2) + PROMOTE_U64(CF),
			PROMOTE_I64(val1) + PROMOTE_I64(val2) + PROMOTE_I64(CF));
		WriteOperand32(inst, inst->Main.Argument1, offset, r);
	}
}

void Processor::Adc_83_ext2(const Instruction *inst)
{
	//ADC r/m16/32, imm8
	u32 offset;
	if (inst->Main.Prefix.OperandSize) {
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = SIGN_EXTEND(8, 16, inst->Main.Inst.Immediat);
		u16 r = val1 + val2 + CF;
		SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2) + PROMOTE_U32(CF),
			PROMOTE_I32(val1) + PROMOTE_I32(val2) + PROMOTE_I32(CF));
		WriteOperand16(inst, inst->Main.Argument1, offset, r);
	} else {
		u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);
		u32 r = val1 + val2 + CF;
		SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2) + PROMOTE_U64(CF),
			PROMOTE_I64(val1) + PROMOTE_I64(val2) + PROMOTE_I64(CF));
		WriteOperand32(inst, inst->Main.Argument1, offset, r);
	}
}

END_NAMESPACE_LOCHSEMU()