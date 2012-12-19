#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Movzx_0FB6(Processor *cpu, const Instruction *inst)
{
    // MOVZX r16/32, r/m8
    if (inst->Main.Prefix.OperandSize) {
        u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand16(inst, inst->Main.Argument1, 0, (u16) val2);
    } else {
        u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand32(inst, inst->Main.Argument1, 0, (u32) val2);
    }
    RET_SUCCESS();
}

LxResult Movzx_0FB7(Processor *cpu, const Instruction *inst)
{
    // MOVZX r32, r/m16
    u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
    cpu->WriteOperand32(inst, inst->Main.Argument1, 0, (u32) val2);
    RET_SUCCESS();
}

LxResult Movsx_0FBE(Processor *cpu, const Instruction *inst)
{
    /**
     * MOVSX r32, r/m8
     */
	if (inst->Main.Prefix.OperandSize) {
		u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2,NULL);
		cpu->WriteOperand16(inst, inst->Main.Argument1, 0, SIGN_EXTEND(8, 16, val2));
	} else {
		u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
		cpu->WriteOperand32(inst, inst->Main.Argument1, 0, SIGN_EXTEND(8, 32, val2));
	}  
    RET_SUCCESS();
}

LxResult Movsx_0FBF(Processor *cpu, const Instruction *inst)
{
    /*
     * MOVSX r32, r/m16
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
    cpu->WriteOperand32(inst, inst->Main.Argument1, 0, SIGN_EXTEND(16, 32, val2));
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()