#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Test_84(const Instruction *inst)
{
    /**
     * TEST r/m8, r8
     */
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 & val2;
    SetFlagsLogic8(r);
    
}

void Processor::Test_85(const Instruction *inst)
{
    /**
     * TEST r/m16, r16
     * TEST r/m32, r32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 r = val1 & val2;
        SetFlagsLogic16(r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 & val2;
        SetFlagsLogic32(r);
    }
    
}

void Processor::Test_A8(const Instruction *inst)
{
    /*
     * TEST AL, imm8
     */
    u8 r = AL & (u8) inst->Main.Inst.Immediat;
    SetFlagsLogic8(r);
    
}

void Processor::Test_A9(const Instruction *inst)
{
    /**
     * TEST EAX, imm32
     */
	if (inst->Main.Prefix.OperandSize) {
		u16 r = EAX & (u16) inst->Main.Inst.Immediat;
		SetFlagsLogic16(r);

	} else {
		u32 r = EAX & inst->Main.Inst.Immediat;
		SetFlagsLogic32(r);
	}
	
}

void Processor::Test_F6_ext0(const Instruction *inst)
{
    /**
     * TEST r/m8, imm8
     */
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = val1 & val2;
    SetFlagsLogic8(r);
    
}

void Processor::Test_F7_ext0(const Instruction *inst)
{
    /**
     * TEST r/m16, imm16
     * TEST r/m32, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        u16 r = val1 & val2;
        SetFlagsLogic16(r);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 r = val1 & val2;
        SetFlagsLogic32(r);
    }
    
}

END_NAMESPACE_LOCHSEMU()