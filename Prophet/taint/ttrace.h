#pragma once
 
#ifndef __PROPHET_TAINT_TTRACE_H__
#define __PROPHET_TAINT_TTRACE_H__
 
#include "prophet.h"
#include "parallel.h"
#include "utilities.h"
#include "instcontext.h"

struct TContext {
    enum Flag {
        OF = 0,
        SF,
        ZF,
        AF,
        PF,
        CF,
        FLAG_COUNT
    };

    static const int    RegCount    = 8;

    u32         Regs[RegCount];
    u32         Eip;
    u32         Eflags;
    MemAccess   Mr;
    MemAccess   Mw;
    InstPtr     Inst;
    int         Tid;
    ThreadID    ExtTid;
    bool        JumpTaken;
};

class TTracer : public MutexSyncObject, public ISerializable {
public:
    TTracer();
    ~TTracer();

    void        Trace(const Processor *cpu);
    void        Clear();

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

private:
    int         m_count;
};
 
#endif // __PROPHET_TAINT_TTRACE_H__