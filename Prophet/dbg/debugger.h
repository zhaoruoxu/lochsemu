#pragma once

#ifndef __PROPHET_DEBUGGER_H__
#define __PROPHET_DEBUGGER_H__

#include "prophet.h"
#include "parallel.h"
#include "utilities.h"
#include "breakpoint.h"
#include "instcontext.h"
#include "event.h"
#include "process.h"

class ProDebugger : public ISerializable {
public:
    static const int MaxThreads = Process::MaximumThreads;
public:
    enum State {
        STATE_RUNNING,
        STATE_RUNNING_NOBP,
        STATE_SINGLESTEP,
        STATE_STEPOVER,
        STATE_STEPOUT,
        STATE_TERMINATED,
    };

    static const char *StateText[];
public:
    ProDebugger(ProEngine *engine);
    ~ProDebugger();

    void        Initialize();
    void        OnPreExecute(PreExecuteEvent &event);
    void        OnProcessPreRun(ProcessPreRunEvent &event);
    void        OnProcessPostLoad(ProcessPostLoadEvent &event);
    void        OnMemRead(MemReadEvent &event);
    void        OnMemWrite(MemWriteEvent &event);
    void        OnThreadCreate(ThreadCreateEvent &event);
    void        OnThreadExit(ThreadExitEvent &event);
    void        OnStepInto();
    void        OnStepOver();
    void        OnStepOut();
    void        OnRun();
    void        OnRunNoBp();
    void        SetCurrentThread(int tid);
    Thread *    GetCurrentThread();
    const Thread *  GetCurrentThread() const;
    Thread *    GetThread(int tid);
    const Thread *  GetThread(int tid) const;
    int         GetCurrentThreadId() const { return m_currTid; }
    int         GetThreadCount() const;
    void        SetState(int tid, State s);
    State       GetState(int tid) const;
    void        AddBreakpoint(u32 eip, const std::string &desc);
    void        ToggleBreakpoint(u32 eip);
    void        RemoveBreakpoint(u32 eip);
    const Breakpoint * GetBreakpoint(u32 eip) const;
    const Breakpoint & GetBreakpointIndex(int index) const { return m_breakpoints[index]; }
    int         GetNumBreakpoints() const { return (int) m_breakpoints.size(); }
    void        OnTerminate();

    void        UpdateInstContext(const Processor *cpu, InstContext *ctx) const;
    void        UpdateTraceContext(const Processor *cpu, TraceContext *ctx, u32 eip) const;

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;
private:
    void        DoPreExecSingleStep(const Processor *cpu, const Instruction *inst);
    void        CheckBreakpoints(const Processor *cpu, const Instruction *inst);
private:
    ProEngine *         m_engine;
    Archive *           m_archive;

    Thread *            m_threads[MaxThreads];

    State               m_state[MaxThreads];
    u32                 m_stepOverEip[MaxThreads];
    Semaphore *         m_semaphore[MaxThreads];
    const Processor *   m_cpu[MaxThreads];
    const Instruction * m_currInst[MaxThreads];

    std::vector<Breakpoint>     m_breakpoints;
    std::vector<MemAccess>      m_mrs[MaxThreads];
    std::vector<MemAccess>      m_mws[MaxThreads];

    int                 m_currTid;  // Current thread id
    bool                m_switchThreadOnBreak;
};

#endif // __PROPHET_DEBUGGER_H__
