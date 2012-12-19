#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Sbb_19(Processor *cpu, const Instruction *inst)
{
	// SBB r/m16/32, r16/32
	u32 offset;
	if (inst->Main.Prefix.OperandSize) {
		u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 r = val1 - val2 - cpu->CF;
		cpu->SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2) - PROMOTE_U32(cpu->CF),
			PROMOTE_I32(val1) - PROMOTE_I32(val2) - PROMOTE_I32(cpu->CF));
		cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
	}
	else{
		u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
		u32 r = val1 - val2 - cpu->CF;
		cpu->SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2) - PROMOTE_U64(cpu->CF),
			PROMOTE_I64(val1) - PROMOTE_I64(val2) - PROMOTE_I64(cpu->CF));
		cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
	}
	RET_SUCCESS();
}

LxResult Sbb_1A(Processor *cpu, const Instruction *inst)
{
    /*
     * SBB r8, r/m8
     */
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2 - cpu->CF;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2) - PROMOTE_U16(cpu->CF),
        PROMOTE_I16(val1) - PROMOTE_I16(val2) - PROMOTE_I16(cpu->CF));
    cpu->WriteOperand8(inst, inst->Main.Argument1, 0, r);
    RET_SUCCESS();
}

LxResult Sbb_1B(Processor *cpu, const Instruction *inst)
{
    /**
     * SBB r16/32, r/m16/32
     */
    if (inst->Main.Prefix.OperandSize) {
		u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, NULL);
		u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 r = val1 - val2 - cpu->CF;
		cpu->SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2) - PROMOTE_U32(cpu->CF),
			PROMOTE_I32(val1) - PROMOTE_I32(val2) - PROMOTE_I32(cpu->CF));
		cpu->WriteOperand16(inst, inst->Main.Argument1, 0, r);
    } 
	else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 - val2 - cpu->CF;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2) - PROMOTE_U64(cpu->CF),
            PROMOTE_I64(val1) - PROMOTE_I64(val2) - PROMOTE_I64(cpu->CF));
        cpu->WriteOperand32(inst, inst->Main.Argument1, 0, r);
    }
    RET_SUCCESS();
}

LxResult Sbb_83_ext3(Processor *cpu, const Instruction *inst)
{
    /*
     * SBB r/m32, imm8 ;  SBB r/m16, imm8
     */
	u32 offset;
    if (inst->Main.Prefix.OperandSize) {		
		u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = SIGN_EXTEND(8, 16, inst->Main.Inst.Immediat);
		u16 r = val1 - val2 - cpu->CF;
		cpu->SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2) - PROMOTE_U32(cpu->CF),
			PROMOTE_I32(val1) - PROMOTE_I32(val2) - PROMOTE_I32(cpu->CF));
		cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    }
	else{
		u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);
		u32 r = val1 - val2 - cpu->CF;
		cpu->SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2) - PROMOTE_U64(cpu->CF),
			PROMOTE_I64(val1) - PROMOTE_I64(val2) - PROMOTE_I64(cpu->CF));
		cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
	}
    RET_SUCCESS();
}


END_NAMESPACE_LOCHSEMU()