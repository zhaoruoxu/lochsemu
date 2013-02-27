#pragma once
 
#ifndef __ARIETIS_INSTCONTEXT_H__
#define __ARIETIS_INSTCONTEXT_H__
 
#include "Arietis.h"
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

    u32                 regs[RegCount];
    Taint4              regTaint[RegCount];

    u32                 Eip;
    Taint               EipTaint;

    u32                 flags[FlagCount];
    Taint               flagTaint[FlagCount];

    std::string         moduleName;
    u32                 moduleImageBase;

    InstPtr             inst;

    InstContext() {
        ZeroMemory(regs, sizeof(regs));
        ZeroMemory(flags, sizeof(flags));
        moduleImageBase = 0;
        inst            = NULL;
    }
};
 
#endif // __ARIETIS_INSTCONTEXT_H__