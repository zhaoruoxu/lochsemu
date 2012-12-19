#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Sub_28(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB r/m8, r8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Sub_29(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB r/m16, r16
     * SUB r/m32, r32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 r = val1 - val2;
        cpu->SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 - val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}

LxResult Sub_2A(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB r8, r/m8
     */
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, NULL);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    cpu->WriteOperand8(inst, inst->Main.Argument1, 0, r);
    RET_SUCCESS();
}

LxResult Sub_2B(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB r16, r/m16
     * SUB r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, NULL);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 r = val1 - val2;
        cpu->SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
        cpu->WriteOperand16(inst, inst->Main.Argument1, 0, r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 r = val1 - val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
        cpu->WriteOperand32(inst, inst->Main.Argument1, 0, r);
    }
    RET_SUCCESS();
}

LxResult Sub_2C(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB AL, imm8
     */
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    u8 r = cpu->AL - val2;
    cpu->SetFlagsArith8(r, PROMOTE_U32(cpu->AL) - PROMOTE_U32(val2),
        PROMOTE_I32(cpu->AL) - PROMOTE_I32(val2));
    cpu->AL = r;
    RET_SUCCESS();
}

LxResult Sub_2D(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB EAX, imm32
     * SUB AX, imm16
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 r = cpu->EAX - val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(cpu->EAX) - PROMOTE_U64(val2),
            PROMOTE_I64(cpu->EAX) - PROMOTE_I64(val2));
        cpu->EAX = r;
    }
    RET_SUCCESS();
}

LxResult Sub_80_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB r/m8, imm8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u8 r = val1 - val2;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Sub_81_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB r/m16, imm16
     * SUB r/m32, imm32
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, &offset);
        u16 r = val1 - val2;
        cpu->SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, &offset);
        u32 r = val1 - val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}

LxResult Sub_83_ext5(Processor *cpu, const Instruction *inst)
{
    /**
     * SUB r/m16, imm8 SE
     * SUB r/m32, imm8 SE
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = SIGN_EXTEND(8, 16, inst->Main.Inst.Immediat);
        u16 r = val1 - val2;
        cpu->SetFlagsArith16(r, PROMOTE_U32(val1) - PROMOTE_U32(val2),
            PROMOTE_I32(val1) - PROMOTE_I32(val2));
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = SIGN_EXTEND(8, 32, inst->Main.Inst.Immediat);
        u32 r = val1 - val2;
        cpu->SetFlagsArith32(r, PROMOTE_U64(val1) - PROMOTE_U64(val2),
            PROMOTE_I64(val1) - PROMOTE_I64(val2));
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}


END_NAMESPACE_LOCHSEMU()