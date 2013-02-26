#pragma once

#ifndef __ARIETIS_DEBUGGER_H__
#define __ARIETIS_DEBUGGER_H__

#include "Arietis.h"
#include "parallel.h"

class ADebugger {
public:
    enum State {
        STATE_RUNNING,
        STATE_SINGLESTEP,
        STATE_STEPOVER,
        STATE_STEPOUT,
        STATE_TERMINATED,
    };
public:
    ADebugger(AEngine *engine);
    ~ADebugger();

    void        Initialize();
    void        OnPreExecute(Processor *cpu, const Instruction *inst);
    //void        OnPostExecute(Processor *cpu, const Instruction *inst);
    void        OnProcPreRun(const Process *proc, const Processor *cpu);
    void        OnProcessPostLoad(const PeLoader *loader);
    void        OnStepInto();
    void        OnStepOver();
    void        OnStepOut();
    void        OnRun();
    void        AddBreakpoint(u32 eip, const std::string &desc);
    void        ToggleBreakpoint(u32 eip);
    void        RemoveBreakpoint(u32 eip);
    const Breakpoint * GetBreakpoint(u32 eip) const;
    void        OnTerminate();

    void        UpdateInstContext(InstContext *ctx) const;
    void        UpdateTraceContext(TraceContext *ctx, u32 eip) const;
private:
    void        DoPreExecSingleStep(const Processor *cpu, const Instruction *inst);
    void        CheckBreakpoints(const Processor *cpu, const Instruction *inst);
    void        AnalyzeCRTEntry(const Processor *cpu, const Instruction *inst);
private:
    State               m_state;
    u32                 m_stepOverEip;

    AEngine *           m_engine;
    Archive *           m_archive;

    Semaphore           m_semaphore;

    const Processor *   m_currProcessor;
    const Instruction * m_currInst;
    bool                m_crtEntryFound;
    u32                 m_mainEntry;
};

#endif // __ARIETIS_DEBUGGER_H__
