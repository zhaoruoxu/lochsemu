#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Retn_C2(Processor *cpu, const Instruction *inst)
{
    /**
     * RETN imm16
     */
    // TODO: #SS(0)
    cpu->EIP = cpu->Pop32();
    u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, NULL);
    cpu->ESP += val1;
    if (cpu->EIP == TERMINATE_EIP)
        cpu->SetExecFlag(LX_EXEC_TERMINATE_EIP);
    RET_SUCCESS();
}

LxResult Ret_C3(Processor *cpu, const Instruction *inst)
{
    /**
     * RET
     */
    cpu->EIP = cpu->Pop32();
    if (cpu->EIP == TERMINATE_EIP)
        cpu->SetExecFlag(LX_EXEC_TERMINATE_EIP);
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()