#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Pop_8F_ext0(const Instruction *inst)
{
    /**
     * POP r/m16
     * POP r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 r = MemRead16(ESP, LX_REG_SS);
        WriteOperand16(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), r);
        ESP += 2;
    } else {
        u32 r = MemRead32(ESP, LX_REG_SS);
        WriteOperand32(inst, inst->Main.Argument1, Offset32(inst->Main.Argument1), r);
        ESP += 4;
    }
    
}

void Processor::Pop_58X(const Instruction *inst)
{
    /**
     * POP r16
     * POP r32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 r = MemRead16(ESP, LX_REG_SS);
        GP_Regs[inst->Main.Inst.Opcode - 0x58].X16 = r;
        ESP += 2;
    } else {
        u32 r = MemRead32(ESP, LX_REG_SS);
        GP_Regs[inst->Main.Inst.Opcode - 0x58].X32 = r;
        ESP += 4;
    }
    
}
void Processor::Popad_61(const Instruction *inst)
{
    /***popad****/
    EDI=MemRead32(ESP, LX_REG_SS);
    ESP+=4;
    ESI=MemRead32(ESP, LX_REG_SS);
    ESP+=4;
    EBP=MemRead32(ESP, LX_REG_SS);
    ESP+=8;
    EBX=MemRead32(ESP, LX_REG_SS);
    ESP+=4;
    EDX=MemRead32(ESP, LX_REG_SS);
    ESP+=4;
    ECX=MemRead32(ESP, LX_REG_SS);
    ESP+=4;
    EAX=MemRead32(ESP, LX_REG_SS);
    ESP+=4;
    
}
void Processor::Popf_9D(const Instruction *inst)
{
    /**
     * POPF
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 r = MemRead32(ESP, LX_REG_SS);
    SetEflags(r);
    ESP += 4;
    
}

END_NAMESPACE_LOCHSEMU()