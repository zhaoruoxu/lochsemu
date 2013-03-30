#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Movzx_0FB6(const Instruction *inst)
{
    // MOVZX r16/32, r/m8
    if (inst->Main.Prefix.OperandSize) {
        u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
        WriteOperand16(inst, inst->Main.Argument1, 0, (u16) val2);
    } else {
        u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
        WriteOperand32(inst, inst->Main.Argument1, 0, (u32) val2);
    }
    
}

void Processor::Movzx_0FB7(const Instruction *inst)
{
    // MOVZX r32, r/m16
    u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
    WriteOperand32(inst, inst->Main.Argument1, 0, (u32) val2);
    
}

void Processor::Movsx_0FBE(const Instruction *inst)
{
    /**
     * MOVSX r32, r/m8
     */
	if (inst->Main.Prefix.OperandSize) {
		u8 val2 = ReadOperand8(inst, inst->Main.Argument2,NULL);
		WriteOperand16(inst, inst->Main.Argument1, 0, SIGN_EXTEND(8, 16, val2));
	} else {
		u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
		WriteOperand32(inst, inst->Main.Argument1, 0, SIGN_EXTEND(8, 32, val2));
	}  
    
}

void Processor::Movsx_0FBF(const Instruction *inst)
{
    /*
     * MOVSX r32, r/m16
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
    WriteOperand32(inst, inst->Main.Argument1, 0, SIGN_EXTEND(16, 32, val2));
    
}

END_NAMESPACE_LOCHSEMU()