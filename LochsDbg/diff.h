#pragma once

#ifndef __LOCHSDBG_DIFF_H__
#define __LOCHSDBG_DIFF_H__

#include "LochsDbg.h"
#include "instruction.h"
#include "tracer.h"

class Diff {
public:
    Diff();
    virtual ~Diff();

    void        Initialize(void);
    void        OnProcessPreRun(const Process *proc, Processor *cpu);
    void        OnProcessorPreExecute(Processor *cpu, const Instruction *inst);
    void        OnProcessorPostExecute(Processor *cpu, const Instruction *inst);
    void        OnProcessPreLoad(PeLoader *loader);

public:
    bool        IsSynced(void) const { return m_synced; }
    bool        IsEnabled(void) const { return m_enabled; }
    void        StepOut(void) { m_flagStepOut = true; }
    void        PrintRefRegisters(void);

private:
    void        OverrideContext(Processor *cpu);
    bool        CompareRuntimeContext(const Processor *cpu, const CONTEXT *ctx);

private:
    RefProcess *        m_refProcess;
    bool                m_enabled;
    bool                m_synced;
    bool                m_enableTraceWhenSync;
    bool                m_breakOnDiff;
    bool                m_flagStepOut;  
    bool                m_breakOnSync;
    PROCESS_INFORMATION*m_pi;
    u32                 m_startAddress;
    DEBUG_EVENT *       m_event;
    u32                 m_lastEip;
    u32                 m_syncStart;        // Custom address to start syncing

    // Overriding settings
    bool                m_overrideFpuCtrl;
    bool                m_overrideStack;
    bool                m_overrideCommonRegs;

    // Diff settings
    bool                m_diffCommonRegs;
    bool                m_diffEip;
    bool                m_diffStackRegs;
};

extern Diff LxDiff;

#endif // __LOCHSDBG_DIFF_H__
