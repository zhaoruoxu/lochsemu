#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Mov_88(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r/m8, r8
     */
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    cpu->WriteOperand8(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val2);
    RET_SUCCESS();
}

LxResult Mov_89(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r/m16/32, r16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand16(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val2);
    } else {
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand32(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val2);
    }
    RET_SUCCESS();
}

LxResult Mov_8A(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r8, r/m8
     */
    u8 val2 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    cpu->WriteOperand8(inst, inst->Main.Argument1, 0, val2);
    RET_SUCCESS();
}

LxResult Mov_8B(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r16/32, r/m16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand16(inst, inst->Main.Argument1, 0, val2);
    } else {
        u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        cpu->WriteOperand32(inst, inst->Main.Argument1, 0, val2);
    }
    RET_SUCCESS();
}

LxResult Mov_8C(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r/m16, sreg
     */
    if (!inst->Main.Prefix.OperandSize) {
        LxWarning("Mov_8C_r/m16_sreg  without OPERAND_16BIT prefix\n");
    }
    NOT_IMPLEMENTED();
    u16 val2 = cpu->Seg_Regs[REG_NUM(inst->Main.Argument2.ArgType)];
    cpu->WriteOperand16(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val2);
    RET_SUCCESS();
}

LxResult Mov_A0(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV AL, moffs8
     */
    cpu->AL = cpu->MemRead8((u32) inst->Main.Argument2.Memory.Displacement);
    RET_SUCCESS();
}

LxResult Mov_A1(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV AX/EAX, moffs16/32
     */
    RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
    if (inst->Main.Prefix.OperandSize) {
        cpu->AX = cpu->MemRead16((u32) inst->Main.Argument2.Memory.Displacement, seg);
    } else {
        cpu->EAX = cpu->MemRead32((u32) inst->Main.Argument2.Memory.Displacement, seg);
    }
    RET_SUCCESS();
}

LxResult Mov_A2(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV moffs8, AL
     */
    RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
    cpu->MemWrite8((u32) inst->Main.Argument1.Memory.Displacement, cpu->AL, seg);
    RET_SUCCESS();
}

LxResult Mov_A3(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV moffs16/32, AX/EAX
     */
    RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
    if (inst->Main.Prefix.OperandSize) {
        cpu->MemWrite16((u32) inst->Main.Argument1.Memory.Displacement, cpu->AX, seg);
    } else {
        cpu->MemWrite32((u32) inst->Main.Argument1.Memory.Displacement, cpu->EAX, seg);
    }
    RET_SUCCESS();
}

LxResult Mov_B0X(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r8, imm8
     */
    *(cpu->GP_Regs8[inst->Main.Inst.Opcode - 0xb0]) = (u8) inst->Main.Inst.Immediat;
    RET_SUCCESS();
}

LxResult Mov_B8X(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r16, imm16
     * MOV r32, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        cpu->GP_Regs[inst->Main.Inst.Opcode - 0xb8].X16 = (u16) inst->Main.Inst.Immediat;
    } else {
        cpu->GP_Regs[inst->Main.Inst.Opcode - 0xb8].X32 = (u32) inst->Main.Inst.Immediat;
    }
    RET_SUCCESS();
}

LxResult Mov_C6_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r/m8, imm8
     */
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    cpu->WriteOperand8(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val2);
    RET_SUCCESS();
}

LxResult Mov_C7_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * MOV r/m16, imm16
     * MOV r/m32, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        cpu->WriteOperand16(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val2);
    } else {
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        cpu->WriteOperand32(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), val2);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()