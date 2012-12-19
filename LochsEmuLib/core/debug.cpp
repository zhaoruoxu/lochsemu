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


//     LxInfo("sizeof(Instruction) = %d\n", sizeof(Instruction));
//     LxInfo("sizeof(UIntPtr) = %d\n", sizeof(UIntPtr));
//     LxInfo("sizeof(DISASM) = %d\n", sizeof(DISASM));
//     LxInfo("sizeof(INSTRUCTION) = %d\n", sizeof(INSTRUCTION));
//     LxInfo("sizeof(INSTRTYPE) = %d\n", sizeof(INSTRTYPE));
//     LxInfo("sizeof(ARGTYPE) = %d\n", sizeof(ARGTYPE));
//     LxInfo("sizeof(PREFIXINFO) = %d\n", sizeof(PREFIXINFO));
//     LxInfo("sizeof(INST) = %d\n", sizeof(INST));
//     LxInfo("sizeof(OPERAND) = %d\n", sizeof(OPERAND));
}

END_NAMESPACE_LOCHSEMU()