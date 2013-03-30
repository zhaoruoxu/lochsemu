#include "stdafx.h"
#include "processor.h"
#include "pemodule.h"
#include "winapi.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Jmp_E9(const Instruction *inst)
{
    /**
     * JMP rel32
     */
    u32 rel = (u32) inst->Aux.op1.immediate;
    EIP += rel;
    
}

void Processor::Jmp_EB(const Instruction *inst)
{
    /**
     * JMP rel8
     */
    u32 rel = SIGN_EXTEND(8, 32, inst->Aux.op1.immediate);
    EIP += rel;
    
}

void Processor::Jmp_FF_ext4(const Instruction *inst)
{
    /**
     * JMP far m16:32
     */
    if (inst->Main.Argument1.ArgType & MEMORY_TYPE) {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        if (LX_IS_WINAPI(val1)) {
            SetExecFlag(LX_EXEC_WINAPI_JMP);
            CallWindowsAPI(this, val1);
        } else {
            // not windows api
            EIP = val1;
        }
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
        EIP = val1;
    }
    
}
END_NAMESPACE_LOCHSEMU()