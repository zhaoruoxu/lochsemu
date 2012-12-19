#include "stdafx.h"
#include "processor.h"
#include "winapi.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Call_E8(Processor *cpu, const Instruction *inst)
{
    /**
     * CALL rel16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        RET_NOT_IMPLEMENTED();
    } else {
        u32 rel32 = (u32) inst->Aux.op1.immediate;
        u32 tempEIP = cpu->EIP + rel32;
        // TODO
        // if tempEIP not in Code Segment then #GP(0)
        // if stack not large enough for 4-byte RA then #SS(0)
        cpu->ESP -= 4;
        cpu->MemWrite32(cpu->ESP, cpu->EIP, LX_REG_SS);
        cpu->EIP = tempEIP;
    }
    RET_SUCCESS();
}

LxResult Call_FF_ext2(Processor *cpu, const Instruction *inst)
{
    /**
     * CALL r/m16
     * CALL r/m32
     */
    if (inst->Main.Prefix.OperandSize)  RET_NOT_IMPLEMENTED();

    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);

    cpu->ESP -= 4;
    cpu->MemWrite32(cpu->ESP, cpu->EIP, LX_REG_SS);
    u32 origEip = cpu->EIP;
    cpu->EIP = val1;

    if (LX_IS_WINAPI(val1)) {
        if (LX_WINAPI_NUM(val1) == 0) {
            LxWarning("Windows API not available at eip = %08x\n", origEip);
        }
        cpu->SetExecFlag(LX_EXEC_WINAPI_CALL);
        CallWindowsAPI(cpu, val1);
        RET_SUCCESS();
    } 
    // TODO

    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()