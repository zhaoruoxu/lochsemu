#pragma once
 
#ifndef __PROPHET_TAINT_TTRACE_H__
#define __PROPHET_TAINT_TTRACE_H__
 
#include "prophet.h"
#include "parallel.h"
#include "utilities.h"
#include "instcontext.h"

struct TContext {

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
    u32         ExecFlag;

    u32         Flag(InstContext::Flag f) const {
        return (Eflags >> f) & 1;
    }

    bool        HasExecFlag(u32 flag) const {
        return (ExecFlag & flag) == flag;
    }

    void        Dump(File &f) const;

//     TContext() {
//         ZeroMemory(this, sizeof(TContext));
//     }
};

class RunTrace : public MutexSyncObject, public ISerializable {
public:
    RunTrace(Protocol *engine);
    ~RunTrace();

    void        Begin();
    void        Trace(const Processor *cpu);
    void        End();
    int         Count() const { return m_count; }
    TContext *  Get(int n) { Assert(n >= 0 && n < m_count); return m_traces + n; }
    const TContext * Get(int n) const { Assert(n >= 0 && n < m_count); return m_traces + n; }

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;
    
    void        Dump(File &f) const;

private:
    int         m_count;
    int         m_maxTraces;
    Protocol *  m_engine;
    TContext *  m_traces;
    bool        m_mergeCallJmp;
};
 
#endif // __PROPHET_TAINT_TTRACE_H__