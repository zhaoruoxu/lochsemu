#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Test_84(Processor *cpu, const Instruction *inst)
{
    /**
     * TEST r/m8, r8
     */
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 & val2;
    cpu->SetFlagsLogic8(r);
    RET_SUCCESS();
}

LxResult Test_85(Processor *cpu, const Instruction *inst)
{
    /**
     * TEST r/m16, r16
     * TEST r/m32, r32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 r = val1 & val2;
        cpu->SetFlagsLogic16(r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 & val2;
        cpu->SetFlagsLogic32(r);
    }
    RET_SUCCESS();
}

LxResult Test_A8(Processor *cpu, const Instruction *inst)
{
    /*
     * TEST AL, imm8
     */
    u8 r = cpu->AL & (u8) inst->Main.Inst.Immediat;
    cpu->SetFlagsLogic8(r);
    RET_SUCCESS();
}

LxResult Test_A9(Processor *cpu, const Instruction *inst)
{
    /**
     * TEST EAX, imm32
     */
	if (inst->Main.Prefix.OperandSize) {
		u16 r = cpu->EAX & (u16) inst->Main.Inst.Immediat;
		cpu->SetFlagsLogic16(r);

	} else {
		u32 r = cpu->EAX & inst->Main.Inst.Immediat;
		cpu->SetFlagsLogic32(r);
	}
	RET_SUCCESS();
}

LxResult Test_F6_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * TEST r/m8, imm8
     */
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = val1 & val2;
    cpu->SetFlagsLogic8(r);
    RET_SUCCESS();
}

LxResult Test_F7_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * TEST r/m16, imm16
     * TEST r/m32, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        u16 r = val1 & val2;
        cpu->SetFlagsLogic16(r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 r = val1 & val2;
        cpu->SetFlagsLogic32(r);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()