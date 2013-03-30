#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Push_50X(const Instruction *inst)
{
    /**
     * PUSH reg32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val = GP_Regs[inst->Main.Inst.Opcode - 0x50].X16;
        ESP -= 2;
        MemWrite16(ESP, val, LX_REG_SS);
    } else {
        u32 val = GP_Regs[inst->Main.Inst.Opcode - 0x50].X32;
        ESP -= 4;
        MemWrite32(ESP, val, LX_REG_SS);
    }
    
}
void Processor::Pushad_60(const Instruction *inst)
{
    u32 temp=ESP;
    ESP-=4;
    MemWrite32(ESP,EAX,LX_REG_SS);
    ESP-=4;
    MemWrite32(ESP,ECX,LX_REG_SS);
    ESP-=4;
    MemWrite32(ESP,EDX,LX_REG_SS);
    ESP-=4;
    MemWrite32(ESP,EBX,LX_REG_SS);
    ESP-=4;
    MemWrite32(ESP,temp,LX_REG_SS);
    ESP-=4;
    MemWrite32(ESP,EBP,LX_REG_SS);
    ESP-=4;
    MemWrite32(ESP,ESI,LX_REG_SS);
    ESP-=4;
    MemWrite32(ESP,EDI,LX_REG_SS);
    
}

void Processor::Push_6A(const Instruction *inst)
{
    /**
     * PUSH imm8
     */
    u8 val1 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u32 t1 = SIGN_EXTEND(8, 32, val1);
    ESP -= 4;
    MemWrite32(ESP, t1, LX_REG_SS);
    
}

void Processor::Push_68(const Instruction *inst)
{
    /**
     * PUSH imm16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = (u16) inst->Main.Inst.Immediat; //ReadOperand16(inst, inst->Main.Argument1, NULL);
        ESP -= 2;
        MemWrite16(ESP, val1, LX_REG_SS);
    } else {
        u32 val1 = (u32) inst->Main.Inst.Immediat; //ReadOperand32(inst, inst->Main.Argument1, NULL);
        ESP -= 4;
        MemWrite32(ESP, val1, LX_REG_SS);
    }
    
}

void Processor::Pushf_9C(const Instruction *inst)
{
    /**
     * PUSHF
     */
    if (inst->Main.Prefix.OperandSize) {
        NOT_IMPLEMENTED();
    }
    u32 val = GetEflags();
    ESP -= 4;
    MemWrite32(ESP, val, LX_REG_SS);
    
}

void Processor::Push_FF_ext6(const Instruction *inst)
{
    /**
     * PUSH r/m16
     * PUSH r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        ESP -= 2;
        MemWrite16(ESP, val1, LX_REG_SS);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        ESP -= 4;
        MemWrite32(ESP, val1, LX_REG_SS);
    }
    
}

END_NAMESPACE_LOCHSEMU()