#include "stdafx.h"
#include "processor.h"
#include "pemodule.h"
#include "winapi.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Jmp_E9(Processor *cpu, const Instruction *inst)
{
    /**
     * JMP rel32
     */
    u32 rel = (u32) inst->Aux.op1.immediate;
    cpu->EIP += rel;
    RET_SUCCESS();
}

LxResult Jmp_EB(Processor *cpu, const Instruction *inst)
{
    /**
     * JMP rel8
     */
    u32 rel = SIGN_EXTEND(8, 32, inst->Aux.op1.immediate);
    cpu->EIP += rel;
    RET_SUCCESS();
}

LxResult Jmp_FF_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * JMP far m16:32
     */
    if (inst->Main.Argument1.ArgType & MEMORY_TYPE) {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
        if (LX_IS_WINAPI(val1)) {
            cpu->SetExecFlag(LX_EXEC_WINAPI_JMP);
            CallWindowsAPI(cpu, val1);
        } else {
            // not windows api
            cpu->EIP = val1;
        }
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
        cpu->EIP = val1;
    }
    RET_SUCCESS();
}
END_NAMESPACE_LOCHSEMU()