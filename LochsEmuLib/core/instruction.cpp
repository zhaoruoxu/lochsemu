#include "stdafx.h"
#include "instruction.h"

BEGIN_NAMESPACE_LOCHSEMU()


bool Instruction::IsRet( const Instruction *inst )
{
    u32 opcode = inst->Main.Inst.Opcode;
    return opcode == 0xc3 || opcode == 0xcb || opcode == 0xc2 || opcode == 0xca;
}

bool Instruction::IsCall( const Instruction *inst )
{
    const char *mnemonics = inst->Main.Inst.Mnemonic;
    return strstr(mnemonics, "call") == mnemonics;
}

END_NAMESPACE_LOCHSEMU()