#pragma once
 
#ifndef __PROPHET_INSTCONTEXT_H__
#define __PROPHET_INSTCONTEXT_H__
 
#include "prophet.h"
#include "taint/taint.h"
#include "static/disassembler.h"

struct MemAccess {
    u32     Addr;
    u32     Len;
    u32     Val;

    MemAccess() { Addr = Len = Val = 0; }
    MemAccess(u32 addr, u32 len, u32 val)
        : Addr(addr), Len(len), Val(val) {}
};

struct InstContext {
    enum Flag {
        OF = 0,
        SF,
        ZF,
        AF,
        PF,
        CF,
        DF,
        //         TF,
        //         IF,
        //         DF,
        //         NT,
        //         RF,
        FLAG_COUNT
    };

    static const int    RegCount    = 8;
    static const int    FlagCount   = FLAG_COUNT;
    static const std::string RegNames[];
    static const std::string FlagNames[];

    u32                 Regs[RegCount];
    u32                 Eip;
    u32                 Flags[FlagCount];

    std::string         ModuleName;
    u32                 ModuleImageBase;

    InstPtr             Inst;
    bool                JumpTaken;

    std::vector<MemAccess>  MRs;
    std::vector<MemAccess>  MWs;

    int                 Tid;
    ThreadID            ExternalTid;

    InstContext() {
        ZeroMemory(Regs, sizeof(Regs));
        ZeroMemory(Flags, sizeof(Flags));
        ModuleImageBase = 0;
        Inst            = NULL;
    }

    void Reset() {
        ZeroMemory(Regs, sizeof(Regs));
        Eip         = 0;
        ZeroMemory(Flags, sizeof(Flags));
        ModuleName  = "";
        ModuleImageBase = 0;
        Inst        = NULL;
        JumpTaken   = false;
        MRs.clear();
        MWs.clear();
    }
};

INLINE bool IsFlagTested(const Instruction *inst, int flag)
{
    u8p pFlag = (u8p) &inst->Main.Inst.Flags;
    return (pFlag[flag] & TE_) != 0;
}

INLINE bool IsFlagModified(const Instruction *inst, int flag)
{
    u8p pFlag = (u8p) &inst->Main.Inst.Flags;
    return (pFlag[flag] & MO_) != 0;
}

INLINE bool IsFlagReset(const Instruction *inst, int flag)
{
    u8p pFlag = (u8p) &inst->Main.Inst.Flags;
    return (pFlag[flag] & RE_) != 0;
}

INLINE bool IsFlagSet(const Instruction *inst, int flag)
{
    u8p pFlag = (u8p) &inst->Main.Inst.Flags;
    return (pFlag[flag] & SE_) != 0;
}

#endif // __PROPHET_INSTCONTEXT_H__