#pragma once

#ifndef __ARIETIS_ENGINE_H__
#define __ARIETIS_ENGINE_H__

#include "Arietis.h"
#include "debugger.h"
#include "tracer.h"
#include "gui/gui.h"
#include "static/disassembler.h"

struct InstContext {
    static const int    RegCount = 8;
    static const int    FlagCount = 11;
    static const std::string FlagNames[];

    u32                 regs[RegCount];
    u32                 flags[FlagCount];
    std::string         moduleName;
    u32                 moduleImageBase;

    Disassembler::Inst  inst;
};

class AEngine {
public:
    AEngine();
    ~AEngine();

    void            Initialize();

    void            SetGuiFrame(ArietisFrame *frame);
    ArietisFrame *  GetGUI() const { Assert(m_gui); return m_gui; }

    void            OnPreExecute(Processor *cpu, const Instruction *inst);
    void            OnPostExecute(Processor *cpu, const Instruction *inst);
    void            OnProcessPreRun(const Process *proc, const Processor *cpu);

    ADebugger *     GetDebugger() { return &m_debugger; }
    ATracer *       GetTracer() { return &m_tracer; }
    Disassembler *  GetDisassembler() { return &m_disassembler; }
    InstContext     GetCurrentInstContext() const;
    void            EnableTracer(bool isEnabled) { m_tracerEnabled = isEnabled; }

    void            ReportBusy(bool isBusy);
    
private:
    ADebugger       m_debugger;
    ATracer         m_tracer;
    Disassembler    m_disassembler;
    ArietisFrame *  m_gui;
    u32             m_currEip;
    i64             m_totalExecuted;

    bool            m_tracerEnabled;
};

#endif // __ARIETIS_ENGINE_H__
