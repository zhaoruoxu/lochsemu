#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Retn_C2(const Instruction *inst)
{
    /**
     * RETN imm16
     */
    // TODO: #SS(0)
    EIP = Pop32();
    u16 val1 = ReadOperand16(inst, inst->Main.Argument1, NULL);
    ESP += val1;
    if (EIP == TERMINATE_EIP)
        SetExecFlag(LX_EXEC_TERMINATE_EIP);
    
}

void Processor::Ret_C3(const Instruction *inst)
{
    /**
     * RET
     */
    EIP = Pop32();
    if (EIP == TERMINATE_EIP)
        SetExecFlag(LX_EXEC_TERMINATE_EIP);
    
}

END_NAMESPACE_LOCHSEMU()