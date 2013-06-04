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
    const char *m= inst->Main.Inst.Mnemonic;
    /*return strstr(mnemonics, "call") == mnemonics;*/
    return m[0] == 'c' && m[1] == 'a' && m[2] == 'l';
}

bool Instruction::IsConditionalJump( const Instruction *inst )
{
    u32 op = inst->Main.Inst.Opcode;
    return (op >= 0x70 && op < 0x80) || (op >= 0x0f80 && op < 0x0f90) || 
        (op == 0xe2) || (op == 0xe3);
}

bool Instruction::IsIndirectJump( const Instruction *inst )
{
    const char *m = inst->Main.Inst.Mnemonic;
    u32 op = inst->Main.Inst.Opcode;
    return op == 0xff && m[0] == 'j' && m[1] == 'm' && m[2] == 'p';
}

bool Instruction::IsCmp( const Instruction *inst )
{
    const char *m = inst->Main.Inst.Mnemonic;
    return m[0] == 'c' && m[1] == 'm' && m[2] == 'p';
}

bool Instruction::IsPop( const Instruction *inst )
{
    const char *m = inst->Main.Inst.Mnemonic;
    return m[0] == 'p' && m[1] == 'o' && m[2] == 'p';
}

bool Instruction::IsCmpOrTest( const Instruction *inst )
{
    const char *m = inst->Main.Inst.Mnemonic;
    return IsCmp(inst) || (m[0] == 't' && m[1] == 'e' && m[2] == 's');
}

END_NAMESPACE_LOCHSEMU()