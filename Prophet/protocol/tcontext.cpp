#include "stdafx.h"
#include "tcontext.h"


u8 TContext::Reg8( uint num, int pos ) const
{
    switch (num) {
    case REG0: { return *( ((u8p) &EAX) + pos ); } break;
    case REG1: { return *( ((u8p) &ECX) + pos); } break;
    case REG2: { return *( ((u8p) &EDX) + pos ); } break;
    case REG3: { return *( ((u8p) &EBX) + pos ); } break;
    default:
        LxFatal("Invalid 8-bit GP register: %x\n", num);
    }
    Assert(!"should never reach here");
    return 0;
}

u16 TContext::Reg16( uint num ) const
{
    switch (num) {
    case REG0: { return AX; } break;
    case REG1: { return CX; } break;
    case REG2: { return DX; } break;
    case REG3: { return BX; } break;
    case REG4: { return SP; } break;
    case REG5: { return BP; } break;
    case REG6: { return SI; } break;
    case REG7: { return DI; } break;
    default:
        LxFatal("Invalid 16-bit GP register: %x\n", num);
    }
    Assert(!"should never reach here");
    return 0;
}

u32 TContext::Reg32( uint num ) const
{
    return Regs[LxRegConvert(num)];
}

u8 TContext::Oper8( const ARGTYPE &oper ) const
{
    if (IsRegArg(oper)) {
        return Reg8(REG_NUM(oper.ArgType), oper.ArgPosition);
    } else if (IsMemoryArg(oper)) {
        return (u8) Mr.Val;
    } else if (IsConstantArg(oper)) {
        return (u8) Inst->Main.Inst.Immediat;
    } else {
        Assert(0);
        return 0;
    }
}

u16 TContext::Oper16( const ARGTYPE &oper ) const
{
    if (IsRegArg(oper)) {
        return Reg16(REG_NUM(oper.ArgType));
    } else if (IsMemoryArg(oper)) {
        return (u16) Mr.Val;
    } else if (IsConstantArg(oper)) {
        return (u16) Inst->Main.Inst.Immediat;
    } else {
        Assert(0);
        return 0;
    }
}

u32 TContext::Oper32( const ARGTYPE &oper ) const
{
    if (IsRegArg(oper)) {
        return Reg32(REG_NUM(oper.ArgType));
    } else if (IsMemoryArg(oper)) {
        return (u32) Mr.Val;
    } else if (IsConstantArg(oper)) {
        return (u32) Inst->Main.Inst.Immediat;
    } else {
        Assert(0);
        return 0;
    }
}

u32 TContext::Oper( const ARGTYPE &oper ) const
{
    if (oper.ArgSize == 32) {
        return Oper32(oper);
    } else if (oper.ArgSize == 16) {
        return Oper16(oper);
    } else if (oper.ArgSize == 8) {
        return Oper8(oper);
    } else {
        Assert(0);
        return 0;
    }
}
