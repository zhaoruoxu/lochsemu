#include "stdafx.h"
#include "processor.h"
#include "winapi.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Call_E8(const Instruction *inst)
{
    /**
     * CALL rel16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        NOT_IMPLEMENTED();
    } else {
        u32 rel32 = (u32) inst->Aux.op1.immediate;
        u32 tempEIP = EIP + rel32;
        // TODO
        // if tempEIP not in Code Segment then #GP(0)
        // if stack not large enough for 4-byte RA then #SS(0)
        ESP -= 4;
        MemWrite32(ESP, EIP, LX_REG_SS);
        EIP = tempEIP;
    }
    
}

void Processor::Call_FF_ext2(const Instruction *inst)
{
    /**
     * CALL r/m16
     * CALL r/m32
     */
    if (inst->Main.Prefix.OperandSize)  NOT_IMPLEMENTED();

    u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);

    ESP -= 4;
    MemWrite32(ESP, EIP, LX_REG_SS);
    u32 origEip = EIP - inst->Length;
    EIP = val1;

    if (LX_IS_WINAPI(val1)) {
        if (LX_WINAPI_NUM(val1) == 0) {
            LxWarning("Windows API not available at eip = %08x\n", origEip);
        }
        SetExecFlag(LX_EXEC_WINAPI_CALL);
        CallWindowsAPI(this, val1);
        
    } 
    // TODO

    
}

END_NAMESPACE_LOCHSEMU()