#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Xchg_87(Processor *cpu, const Instruction *inst)
{
    /*
     * XCHG r/m32, r32
     * XCHG r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 offset1, offset2;
    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset1);
    u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, &offset2);
    cpu->WriteOperand32(inst, inst->Main.Argument2, offset2, val1);
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset1, val2);

    RET_SUCCESS();
}

LxResult Xchg_90X(Processor *cpu, const Instruction *inst)
{
    /**
     * XCHG EAX, r32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 val2 = cpu->GP_Regs[inst->Main.Inst.Opcode - 0x90].X32;
    u32 temp = cpu->EAX;
    cpu->EAX = val2;
    cpu->GP_Regs[inst->Main.Inst.Opcode - 0x90].X32 = temp;
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()