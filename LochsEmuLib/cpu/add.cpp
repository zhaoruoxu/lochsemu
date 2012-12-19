#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Add_00(Processor *cpu, const Instruction *inst)
{
    /**
     * ADD r/m8, r8
     */
    u32 offset = 0;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    
    u8 r = val1 + val2;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2), PROMOTE_I16(val1) + PROMOTE_I16(val2));

    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Add_01(Processor *cpu, const Instruction *inst)
{
    /**
     * ADD r/m16/32 r16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        // 16-bit operands
        u32 offset = 0;
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
        
        u16 r = val1 + val2;
        cpu->SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2), PROMOTE_I32(val1) + PROMOTE_I32(val2));

        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        // 32-bit operands
        u32 offset = 0;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, &offset);
        
        u32 r = val1 + val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2), PROMOTE_I64(val1) + PROMOTE_I64(val2));
        
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }

    RET_SUCCESS();
}

LxResult Add_02(Processor *cpu, const Instruction *inst)
{
    /**
     * ADD r8, r/m8
     */
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 + val2;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2),
        PROMOTE_I16(val1) + PROMOTE_I16(val2));
    cpu->WriteOperand8(inst, inst->Main.Argument1, 0, r);
    RET_SUCCESS();
}

LxResult Add_03(Processor *cpu, const Instruction *inst)
{
    /**
     * ADD r16/32, r/m16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        RET_FAIL(LX_RESULT_NOT_IMPLEMENTED);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 + val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2),
            PROMOTE_I64(val1) + PROMOTE_I64(val2));
        cpu->WriteOperand32(inst, inst->Main.Argument1, 0, r);
    }
    RET_SUCCESS();
}

LxResult Add_04(Processor *cpu, const Instruction *inst)
{
    // ADD AL, imm8
    u8 val1 = cpu->AL;
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = val1 + val2;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2),
        PROMOTE_I16(val1) + PROMOTE_I16(val2));
    cpu->AL = r;
    RET_SUCCESS();
}

LxResult Add_05(Processor *cpu, const Instruction *inst)
{
    /**
     * ADD EAX, imm32
     */
	if (inst->Main.Prefix.OperandSize){
		u16 val1=cpu->AX;
		u16 val2=(u16)inst->Main.Inst.Immediat;
		u16 r=val1+val2;
		cpu->SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2), PROMOTE_I32(val1) + PROMOTE_I32(val2));
		cpu->AX=r;
	}
	else{
		u32 val1 = cpu->EAX;
		u32 val2 = (u32) inst->Main.Inst.Immediat;
		u32 r = val1 + val2;
		cpu->SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2),
			PROMOTE_I64(val1) + PROMOTE_I64(val2));
		cpu->EAX = r;
	}
    RET_SUCCESS();
}

LxResult Add_80_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * ADD r/m8, imm8
     */
    u32 offset = 0;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;

    u8 r = val1 + val2;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) + PROMOTE_U16(val2),
        PROMOTE_I16(val1) + PROMOTE_I16(val2));
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Add_81_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * ADD r/m16, imm16
     * ADD r/m32, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        u32 offset = 0;
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = (u16) inst->Main.Inst.Immediat;

        u16 r = val1 + val2;
        cpu->SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2),
            PROMOTE_I32(val1) + PROMOTE_I32(val2));
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 offset = 0;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = (u32) inst->Main.Inst.Immediat;

        u32 r = val1 + val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2),
            PROMOTE_I64(val1) + PROMOTE_I64(val2));
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}   

LxResult Add_83_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * ADD r/m16, imm8
     * ADD r/m32, imm8
     */
    if (inst->Main.Prefix.OperandSize) {
        u32 offset = 0;
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = SIGN_EXTEND(8, 16, inst->Main.Inst.Immediat);

        u16 r = val1 + val2;
        cpu->SetFlagsArith16(r, PROMOTE_U32(val1) + PROMOTE_U32(val2),
            PROMOTE_I32(val1) + PROMOTE_I32(val2));
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 offset = 0;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);

        u32 r = val1 + val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) + PROMOTE_U64(val2),
            PROMOTE_I64(val1) + PROMOTE_I64(val2));
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()