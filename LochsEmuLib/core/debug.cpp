#include "stdafx.h"
#include "debug.h"
#include "coprocessor.h"
#include "instruction.h"

BEGIN_NAMESPACE_LOCHSEMU()

DebugContext LxDebugContext;

void LX_API LochsDebuggingRoutine()
{

//     for (int i = 0; i < 0x100; i++) {
//         printf("/* 0x%02X */NULL,\n", i);
//     }
//     LxInfo("exceptionrecord: %d  %x\n", sizeof(EXCEPTION_RECORD), sizeof(EXCEPTION_RECORD));
//     LxInfo("context: %d  %x\n", sizeof(CONTEXT), sizeof(CONTEXT));


    LxDebug("sizeof(Instruction) = %d\n", sizeof(Instruction));
    LxDebug("sizeof(UIntPtr) = %d\n", sizeof(UIntPtr));
    LxDebug("sizeof(DISASM) = %d\n", sizeof(DISASM));
    LxDebug("sizeof(INSTRUCTION) = %d\n", sizeof(INSTRUCTION));
    LxDebug("sizeof(INSTRTYPE) = %d\n", sizeof(INSTRTYPE));
    LxDebug("sizeof(ARGTYPE) = %d\n", sizeof(ARGTYPE));
    LxDebug("sizeof(PREFIXINFO) = %d\n", sizeof(PREFIXINFO));
    LxDebug("sizeof(INST) = %d\n", sizeof(INST));
    LxDebug("sizeof(OPERAND) = %d\n", sizeof(OPERAND));
    LxDebug("sizeof(DEBUG_EVENT) = %d\n", sizeof(DEBUG_EVENT));
}

END_NAMESPACE_LOCHSEMU()