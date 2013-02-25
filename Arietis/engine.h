#pragma once

#ifndef __ARIETIS_ENGINE_H__
#define __ARIETIS_ENGINE_H__

#include "Arietis.h"
#include "debugger.h"
#include "tracer.h"
#include "gui/gui.h"
#include "static/disassembler.h"
#include "taint/taintengine.h"


// struct MemContext {
//     u32             Base;
//     u32             Size;
//     std::string     Desc;
//     std::string     ModuleName;
// };

class AEngine {
public:
    AEngine();
    ~AEngine();

    void            Initialize(Emulator *emu);
    void            SaveConfig();

    void            SetGuiFrame(ArietisFrame *frame);
    ArietisFrame *  GetGUI() const { Assert(m_gui); return m_gui; }

    void            OnPreExecute(Processor *cpu, const Instruction *inst);
    void            OnPostExecute(Processor *cpu, const Instruction *inst);
    void            OnProcessPreRun(const Process *proc, const Processor *cpu);
    void            OnProcessPostLoad(const PeLoader *loader);

    Emulator *      GetEmulator() { return m_emulator; }
    ADebugger *     GetDebugger() { return &m_debugger; }
    ATracer *       GetTracer() { return &m_tracer; }
    Disassembler *  GetDisassembler() { return &m_disassembler; }
    TaintEngine *   GetTaintEngine() { return &m_taint; }
    //AArchive &      GetArchive() { return m_archive; }
    InstContext     GetCurrentInstContext() const;
    void            EnableTracer(bool isEnabled) { m_tracerEnabled = isEnabled; }
    bool            IsTracerEnabled() const { return m_tracerEnabled; }
    //void            Persist();

    void            ReportBusy(bool isBusy);
    void            Terminate();
    
private:
    void            Intro() const;

private:
    bool            m_enabled;
    Emulator *      m_emulator;

    ADebugger       m_debugger;
    ATracer         m_tracer;
    Disassembler    m_disassembler;
    TaintEngine     m_taint;
    ArietisFrame *  m_gui;
    //AArchive        m_archive;
    u32             m_currEip;
    i64             m_totalExecuted;

    bool            m_tracerEnabled;
    bool            m_skipDllEntries;
    bool            m_mainEntryEntered;

    //std::string     m_archiveFilePath;
};

#endif // __ARIETIS_ENGINE_H__
