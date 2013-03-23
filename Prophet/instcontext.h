#pragma once
 
#ifndef __PROPHET_INSTCONTEXT_H__
#define __PROPHET_INSTCONTEXT_H__
 
#include "prophet.h"
#include "taint/taint.h"
#include "static/disassembler.h"


struct InstContext {
    enum Flag {
        OF = 0,
        SF,
        ZF,
        AF,
        PF,
        CF,
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
    Taint4              RegTaint[RegCount];

    u32                 Eip;
    Taint1              EipTaint;

    u32                 Flags[FlagCount];
    Taint1              FlagTaint[FlagCount];

    std::string         ModuleName;
    u32                 ModuleImageBase;

    InstPtr             Inst;
    bool                JumpTaken;

    InstContext() {
        ZeroMemory(Regs, sizeof(Regs));
        ZeroMemory(Flags, sizeof(Flags));
        ModuleImageBase = 0;
        Inst            = NULL;
    }

    void Reset() {
        ZeroMemory(Regs, sizeof(Regs));
        ZeroMemory(RegTaint, sizeof(RegTaint));
        Eip         = 0;
        EipTaint.ResetAll();
        ZeroMemory(Flags, sizeof(Flags));
        ZeroMemory(FlagTaint, sizeof(FlagTaint));
        ModuleName  = "";
        ModuleImageBase = 0;
        Inst        = NULL;
        JumpTaken   = false;
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