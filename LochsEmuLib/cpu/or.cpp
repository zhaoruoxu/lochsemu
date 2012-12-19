#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Or_08(Processor *cpu, const Instruction *inst)
{
    /**
     * OR r/m8, r8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 | val2;
    cpu->SetFlagsLogic8(r);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Or_09(Processor *cpu, const Instruction *inst)
{
    /**
     * OR r/m32, r32
     * OR r/m16, r16
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);

        u16 r = val1 | val2;
        cpu->SetFlagsLogic16(r);
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);

        u32 r = val1 | val2;
        cpu->SetFlagsLogic32(r);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}

LxResult Or_0A(Processor *cpu, const Instruction *inst)
{
    /**
     * OR r8, r/m8
     */
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 | val2;
    cpu->SetFlagsLogic8(r);
    cpu->WriteOperand8(inst, inst->Main.Argument1, 0, r);
    RET_SUCCESS();
}

LxResult Or_0B(Processor *cpu, const Instruction *inst)
{
    /**
     * OR r16, r/m16
     * OR r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);

        u16 r = val1 | val2;
        cpu->SetFlagsLogic16(r);
        cpu->WriteOperand16(inst, inst->Main.Argument1, 0, r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);

        u32 r = val1 | val2;
        cpu->SetFlagsLogic32(r);
        cpu->WriteOperand32(inst, inst->Main.Argument1, 0, r);
    }
    RET_SUCCESS();
}

LxResult Or_0C(Processor *cpu, const Instruction *inst)
{
	//OR AL, imm8
	u8 r = cpu->AL | (u8) inst->Main.Inst.Immediat;
	cpu->SetFlagsLogic8(r);
	cpu->AL=r;
	RET_SUCCESS();
}

LxResult Or_0D(Processor *cpu, const Instruction *inst)
{
    /**
     * OR EAX, imm32
     */
	if (inst->Main.Prefix.OperandSize){
		u16 r=cpu->AX | (u16) inst->Main.Inst.Immediat;
		cpu->SetFlagsLogic16(r);
		cpu->AX=r;
	}
	else{
		u32 r = cpu->EAX | (u32) inst->Main.Inst.Immediat;
		cpu->SetFlagsLogic32(r);
		cpu->EAX = r;
	}
    RET_SUCCESS();
}

LxResult Or_80_ext1(Processor *cpu, const Instruction *inst)
{
    /**
     * OR r/m8, imm8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;

    u8 r = val1 | val2;
    cpu->SetFlagsLogic8(r);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Or_81_ext1(Processor *cpu, const Instruction *inst)
{
    /**
     * OR r/m32, imm32
     */
	u32 offset;
	if (inst->Main.Prefix.OperandSize) {
		u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = (u16) inst->Main.Inst.Immediat;

		u16 r = val1 | val2;
		cpu->SetFlagsLogic16(r);
		cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
	} else {
		u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = (u32) inst->Main.Inst.Immediat;

		u32 r = val1 | val2;
		cpu->SetFlagsLogic32(r);
		cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);		
	}
	RET_SUCCESS();
}

LxResult Or_83_ext1(Processor *cpu, const Instruction *inst)
{
    /**
     * OR r/m32, imm8
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);

        u16 r = val1 | val2;
        cpu->SetFlagsLogic16(r);
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);

        u32 r = val1 | val2;
        cpu->SetFlagsLogic32(r);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()