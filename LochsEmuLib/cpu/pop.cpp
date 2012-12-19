#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Pop_8F_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * POP r/m16
     * POP r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 r = cpu->MemRead16(cpu->ESP, LX_REG_SS);
        cpu->WriteOperand16(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), r);
        cpu->ESP += 2;
    } else {
        u32 r = cpu->MemRead32(cpu->ESP, LX_REG_SS);
        cpu->WriteOperand32(inst, inst->Main.Argument1, cpu->Offset32(inst->Main.Argument1), r);
        cpu->ESP += 4;
    }
    RET_SUCCESS();
}

LxResult Pop_58X(Processor *cpu, const Instruction *inst)
{
    /**
     * POP r16
     * POP r32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 r = cpu->MemRead16(cpu->ESP, LX_REG_SS);
        cpu->GP_Regs[inst->Main.Inst.Opcode - 0x58].X16 = r;
        cpu->ESP += 2;
    } else {
        u32 r = cpu->MemRead32(cpu->ESP, LX_REG_SS);
        cpu->GP_Regs[inst->Main.Inst.Opcode - 0x58].X32 = r;
        cpu->ESP += 4;
    }
    RET_SUCCESS();
}
LxResult Popad_61(Processor *cpu, const Instruction *inst)
{
    /***popad****/
    cpu->EDI=cpu->MemRead32(cpu->ESP, LX_REG_SS);
    cpu->ESP+=4;
    cpu->ESI=cpu->MemRead32(cpu->ESP, LX_REG_SS);
    cpu->ESP+=4;
    cpu->EBP=cpu->MemRead32(cpu->ESP, LX_REG_SS);
    cpu->ESP+=8;
    cpu->EBX=cpu->MemRead32(cpu->ESP, LX_REG_SS);
    cpu->ESP+=4;
    cpu->EDX=cpu->MemRead32(cpu->ESP, LX_REG_SS);
    cpu->ESP+=4;
    cpu->ECX=cpu->MemRead32(cpu->ESP, LX_REG_SS);
    cpu->ESP+=4;
    cpu->EAX=cpu->MemRead32(cpu->ESP, LX_REG_SS);
    cpu->ESP+=4;
    RET_SUCCESS();
}
LxResult Popf_9D(Processor *cpu, const Instruction *inst)
{
    /**
     * POPF
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 r = cpu->MemRead32(cpu->ESP, LX_REG_SS);
    cpu->SetEflags(r);
    cpu->ESP += 4;
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()