#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Push_50X(Processor *cpu, const Instruction *inst)
{
    /**
     * PUSH reg32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val = cpu->GP_Regs[inst->Main.Inst.Opcode - 0x50].X16;
        cpu->ESP -= 2;
        cpu->MemWrite16(cpu->ESP, val, LX_REG_SS);
    } else {
        u32 val = cpu->GP_Regs[inst->Main.Inst.Opcode - 0x50].X32;
        cpu->ESP -= 4;
        cpu->MemWrite32(cpu->ESP, val, LX_REG_SS);
    }
    RET_SUCCESS();
}
LxResult Pushad_60(Processor *cpu, const Instruction *inst)
{
    u32 temp=cpu->ESP;
    cpu->ESP-=4;
    cpu->MemWrite32(cpu->ESP,cpu->EAX,LX_REG_SS);
    cpu->ESP-=4;
    cpu->MemWrite32(cpu->ESP,cpu->ECX,LX_REG_SS);
    cpu->ESP-=4;
    cpu->MemWrite32(cpu->ESP,cpu->EDX,LX_REG_SS);
    cpu->ESP-=4;
    cpu->MemWrite32(cpu->ESP,cpu->EBX,LX_REG_SS);
    cpu->ESP-=4;
    cpu->MemWrite32(cpu->ESP,temp,LX_REG_SS);
    cpu->ESP-=4;
    cpu->MemWrite32(cpu->ESP,cpu->EBP,LX_REG_SS);
    cpu->ESP-=4;
    cpu->MemWrite32(cpu->ESP,cpu->ESI,LX_REG_SS);
    cpu->ESP-=4;
    cpu->MemWrite32(cpu->ESP,cpu->EDI,LX_REG_SS);
    RET_SUCCESS();
}

LxResult Push_6A(Processor *cpu, const Instruction *inst)
{
    /**
     * PUSH imm8
     */
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u32 t1 = SIGN_EXTEND(8, 32, val1);
    cpu->ESP -= 4;
    cpu->MemWrite32(cpu->ESP, t1, LX_REG_SS);
    RET_SUCCESS();
}

LxResult Push_68(Processor *cpu, const Instruction *inst)
{
    /**
     * PUSH imm16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = (u16) inst->Main.Inst.Immediat; //ReadOperand16(inst, inst->Main.Argument1, NULL);
        cpu->ESP -= 2;
        cpu->MemWrite16(cpu->ESP, val1, LX_REG_SS);
    } else {
        u32 val1 = (u32) inst->Main.Inst.Immediat; //ReadOperand32(inst, inst->Main.Argument1, NULL);
        cpu->ESP -= 4;
        cpu->MemWrite32(cpu->ESP, val1, LX_REG_SS);
    }
    RET_SUCCESS();
}

LxResult Pushf_9C(Processor *cpu, const Instruction *inst)
{
    /**
     * PUSHF
     */
    if (inst->Main.Prefix.OperandSize) {
        RET_NOT_IMPLEMENTED();
    }
    u32 val = cpu->GetEflags();
    cpu->ESP -= 4;
    cpu->MemWrite32(cpu->ESP, val, LX_REG_SS);
    RET_SUCCESS();
}

LxResult Push_FF_ext6(Processor *cpu, const Instruction *inst)
{
    /**
     * PUSH r/m16
     * PUSH r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
        cpu->ESP -= 2;
        cpu->MemWrite16(cpu->ESP, val1, LX_REG_SS);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        cpu->ESP -= 4;
        cpu->MemWrite32(cpu->ESP, val1, LX_REG_SS);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()