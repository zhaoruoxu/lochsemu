#pragma once
 
#ifndef __PROPHET_PROTOCOL_TCONTEXT_H__
#define __PROPHET_PROTOCOL_TCONTEXT_H__
 
#include "prophet.h"
#include "instcontext.h"

struct TContext {

    static const int    RegCount    = 8;

    union {
        u32 Regs[RegCount];
        struct {
            union { u32 EAX; u16 AX; struct { u8 AL; u8 AH; }; };
            union { u32 ECX; u16 CX; struct { u8 CL; u8 CH; }; };
            union { u32 EDX; u16 DX; struct { u8 DL; u8 DH; }; };
            union { u32 EBX; u16 BX; struct { u8 BL; u8 BH; }; };
            union { u32 ESP; u16 SP; };
            union { u32 EBP; u16 BP; };
            union { u32 ESI; u16 SI; };
            union { u32 EDI; u16 DI; };
        };
    };

    u32         Eip;
    u32         Eflags;
    MemAccess   Mr;
    MemAccess   Mw;
    InstPtr     Inst;
    int         Tid;
    ThreadID    ExtTid;
    bool        JumpTaken;
    u32         ExecFlag;

    u32         Flag(InstContext::Flag f) const {
        return (Eflags >> f) & 1;
    }

    bool        HasExecFlag(u32 flag) const {
        return (ExecFlag & flag) == flag;
    }

    void        Dump(File &f) const;

    u8      Reg8(uint num, int pos) const;
    u16     Reg16(uint num) const;
    u32     Reg32(uint num) const;
    u8      Oper8(const ARGTYPE &oper) const;
    u16     Oper16(const ARGTYPE &oper) const;
    u32     Oper32(const ARGTYPE &oper) const;
    u32     Oper(const ARGTYPE &oper) const;
};

#endif // __PROPHET_PROTOCOL_TCONTEXT_H__