#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Mov_88(const Instruction *inst)
{
    /**
     * MOV r/m8, r8
     */
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    WriteOperand8(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val2);
    
}

void Processor::Mov_89(const Instruction *inst)
{
    /**
     * MOV r/m16/32, r16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        WriteOperand16(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val2);
    } else {
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        WriteOperand32(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val2);
    }
    
}

void Processor::Mov_8A(const Instruction *inst)
{
    /**
     * MOV r8, r/m8
     */
    u8 val2 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    WriteOperand8(inst, inst->Main.Argument1, 0, val2);
    
}

void Processor::Mov_8B(const Instruction *inst)
{
    /**
     * MOV r16/32, r/m16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        WriteOperand16(inst, inst->Main.Argument1, 0, val2);
    } else {
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        WriteOperand32(inst, inst->Main.Argument1, 0, val2);
    }
    
}

void Processor::Mov_8C(const Instruction *inst)
{
    /**
     * MOV r/m16, sreg
     */
    if (!inst->Main.Prefix.OperandSize) {
        LxWarning("Mov_8C_r/m16_sreg  without OPERAND_16BIT prefix\n");
    }
    NOT_IMPLEMENTED();
    u16 val2 = Seg_Regs[REG_NUM(inst->Main.Argument2.ArgType)];
    WriteOperand16(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val2);
    
}

void Processor::Mov_A0(const Instruction *inst)
{
    /**
     * MOV AL, moffs8
     */
    AL = MemRead8((u32) inst->Main.Argument2.Memory.Displacement);
    
}

void Processor::Mov_A1(const Instruction *inst)
{
    /**
     * MOV AX/EAX, moffs16/32
     */
    RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
    if (inst->Main.Prefix.OperandSize) {
        AX = MemRead16((u32) inst->Main.Argument2.Memory.Displacement, seg);
    } else {
        EAX = MemRead32((u32) inst->Main.Argument2.Memory.Displacement, seg);
    }
    
}

void Processor::Mov_A2(const Instruction *inst)
{
    /**
     * MOV moffs8, AL
     */
    RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
    MemWrite8((u32) inst->Main.Argument1.Memory.Displacement, AL, seg);
    
}

void Processor::Mov_A3(const Instruction *inst)
{
    /**
     * MOV moffs16/32, AX/EAX
     */
    RegSeg seg = inst->Main.Prefix.FSPrefix ? LX_REG_FS : LX_REG_DS;
    if (inst->Main.Prefix.OperandSize) {
        MemWrite16((u32) inst->Main.Argument1.Memory.Displacement, AX, seg);
    } else {
        MemWrite32((u32) inst->Main.Argument1.Memory.Displacement, EAX, seg);
    }
    
}

void Processor::Mov_B0X(const Instruction *inst)
{
    /**
     * MOV r8, imm8
     */
    *(GP_Regs8[inst->Main.Inst.Opcode - 0xb0]) = (u8) inst->Main.Inst.Immediat;
    
}

void Processor::Mov_B8X(const Instruction *inst)
{
    /**
     * MOV r16, imm16
     * MOV r32, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        GP_Regs[inst->Main.Inst.Opcode - 0xb8].X16 = (u16) inst->Main.Inst.Immediat;
    } else {
        GP_Regs[inst->Main.Inst.Opcode - 0xb8].X32 = (u32) inst->Main.Inst.Immediat;
    }
    
}

void Processor::Mov_C6_ext0(const Instruction *inst)
{
    /**
     * MOV r/m8, imm8
     */
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    WriteOperand8(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val2);
    
}

void Processor::Mov_C7_ext0(const Instruction *inst)
{
    /**
     * MOV r/m16, imm16
     * MOV r/m32, imm32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        WriteOperand16(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val2);
    } else {
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        WriteOperand32(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), val2);
    }
    
}

END_NAMESPACE_LOCHSEMU()