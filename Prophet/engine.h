#pragma once

#ifndef __PROPHET_ENGINE_H__
#define __PROPHET_ENGINE_H__

#include "prophet.h"
#include "archive.h"
#include "statistics.h"
#include "dbg/debugger.h"
#include "dbg/tracer.h"
#include "gui/gui.h"
#include "static/disassembler.h"
#include "plugin/plugin.h"
#include "protocol/protocol.h"


class ProEngine {
public:
    ProEngine();
    ~ProEngine();

    void            Initialize(Emulator *emu);
    //void            SaveConfig();
    void            SaveArchive();

    void            SetGuiFrame(ProphetFrame *frame);
    ProphetFrame *  GetGUI() const { Assert(m_gui); return m_gui; }
    void            NoGUI();
    bool            GUIEnabled() const { return !m_nogui; }

    void            OnPreExecute        (Processor *cpu, const Instruction *inst);
    void            OnPostExecute       (Processor *cpu, const Instruction *inst);
    void            OnMemRead           (const Processor *cpu, u32 addr, u32 nbytes, cpbyte data);
    void            OnMemWrite          (const Processor *cpu, u32 addr, u32 nbytes, cpbyte data);
    void            OnProcessPreRun     (const Process *proc, const Processor *cpu);
    void            OnProcessPostRun    (const Process *proc);
    void            OnProcessPreLoad    (PeLoader *loader);
    void            OnProcessPostLoad   (PeLoader *loader);
    void            OnWinapiPreCall     (Processor *cpu, uint apiIndex);
    void            OnWinapiPostCall    (Processor *cpu, uint apiIndex);
    void            OnThreadCreate      (Thread *thrd);
    void            OnThreadExit        (Thread *thrd);

    Emulator *      GetEmulator() { return m_emulator; }
    ProDebugger *   GetDebugger() { return &m_debugger; }
    ProTracer *     GetTracer() { return &m_tracer; }
    Disassembler *  GetDisassembler() { return &m_disassembler; }
    //TaintEngine *   GetTaintEngine() { return &m_taint; }
    Archive *       GetArchive() { return &m_archive; }
    ProPluginManager * GetPluginManager() { return &m_plugins; }
    Protocol *      GetProtocol() { return &m_protocol; }
    Statistics *    GetStatistics() { return &m_statistics; }

    const Emulator *    GetEmulator() const { return m_emulator; }
    const ProDebugger * GetDebugger() const { return &m_debugger; }
    const ProTracer *   GetTracer() const { return &m_tracer; }
    const Disassembler *GetDisassembler() const { return &m_disassembler; }
    //const TaintEngine * GetTaintEngine() const { return &m_taint; }
    const Archive *     GetArchive() const { return &m_archive; }
    const ProPluginManager* GetPluginManager() const { return &m_plugins; }
    const Protocol *    GetProtocol() const { return &m_protocol; }
    const Statistics *  GetStatistics() const { return &m_statistics; }

    void            GetInstContext(const Processor *cpu, InstContext *ctx) const;
    void            GetTraceContext(const Processor *cpu, TraceContext *ctx, u32 eip) const;

    void            BreakOnNextInst(const char *desc);

    void            ReportBusy(bool isBusy);
    void            Terminate();
    void            UpdateGUI();    // non-rendering part
    void            RefreshGUI();   // rendering part
    //void            UpdateCpuData(const InstSection *insts);
    i64             GetInstExecuted() const { return m_instExecuted; }

    std::string     GetArchiveDir() const { return m_archiveDir; }
    
private:
    void            Intro() const;
    void            CreateArchiveDirectory();
    void            LoadArchive(const char *moduleName);

private:
    bool            m_enabled;
    Emulator *      m_emulator;

    Statistics      m_statistics;
    ProDebugger     m_debugger;
    ProTracer       m_tracer;
    Disassembler    m_disassembler;
    //TaintEngine     m_taint;
    ProphetFrame *  m_gui;
    Archive         m_archive;
    ProPluginManager    m_plugins;
    Protocol        m_protocol;


    //bool            m_skipDllEntries;
    //bool            m_mainEntryEntered;

    std::string     m_archivePath;
    std::string     m_archiveDir;
    bool            m_isArchiveLoaded;
    i64             m_instExecuted;
    bool            m_nogui;
};

#endif // __PROPHET_ENGINE_H__
