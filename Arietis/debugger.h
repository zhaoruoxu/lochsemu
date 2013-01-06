#pragma once

#ifndef __ARIETIS_DEBUGGER_H__
#define __ARIETIS_DEBUGGER_H__

#include "Arietis.h"
#include "parallel.h"


class Debugger {
public:
    enum State {
        STATE_RUNNING,
        STATE_SINGLESTEP,
        STATE_STEPOVER,
        STATE_STEPOUT,
        STATE_TERMINATED,
    };
public:
    Debugger(ArietisEngine *engine);
    ~Debugger();

    void        Initialize();
    void        OnPreExecute(Processor *cpu, const Instruction *inst);
    void        OnPostExecute(Processor *cpu, const Instruction *inst);
    void        OnProcPreRun(const Process *proc, const Processor *cpu);
    void        OnStepInto();
    void        OnStepOver();
    void        OnStepOut();
    void        OnRun();
    void        OnToggleBreakpoint();

private:
    void        DoSingleStep(const Processor *cpu, const Instruction *inst);
    void        CheckBreakpoints(const Processor *cpu, const Instruction *inst);
private:
    State               m_state;
    u32                 m_stepOverEip;

    ArietisEngine *     m_engine;
    Semaphore           m_semaphore;

    const Processor *   m_currProcessor;
    const Instruction * m_currInst;
};

#endif // __ARIETIS_DEBUGGER_H__
