#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Xchg_87(const Instruction *inst)
{
    /*
     * XCHG r/m32, r32
     * XCHG r32, r/m32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 offset1, offset2;
    u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset1);
    u32 val2 = ReadOperand32(inst, inst->Main.Argument2, &offset2);
    WriteOperand32(inst, inst->Main.Argument2, offset2, val1);
    WriteOperand32(inst, inst->Main.Argument1, offset1, val2);

    
}

void Processor::Xchg_90X(const Instruction *inst)
{
    /**
     * XCHG EAX, r32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 val2 = GP_Regs[inst->Main.Inst.Opcode - 0x90].X32;
    u32 temp = EAX;
    EAX = val2;
    GP_Regs[inst->Main.Inst.Opcode - 0x90].X32 = temp;
    
}

END_NAMESPACE_LOCHSEMU()