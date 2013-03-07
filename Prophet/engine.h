#pragma once

#ifndef __PROPHET_ENGINE_H__
#define __PROPHET_ENGINE_H__

#include "Prophet.h"
#include "archive.h"
#include "dbg/debugger.h"
#include "dbg/tracer.h"
#include "gui/gui.h"
#include "static/disassembler.h"
#include "taint/taintengine.h"
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

    void            OnPreExecute        (Processor *cpu, const Instruction *inst);
    void            OnPostExecute       (Processor *cpu, const Instruction *inst);
    void            OnMemRead           (const Processor *cpu, u32 addr, u32 nbytes, cpbyte data);
    void            OnMemWrite          (const Processor *cpu, u32 addr, u32 nbytes, cpbyte data);
    void            OnProcessPreRun     (const Process *proc, const Processor *cpu);
    void            OnProcessPostRun    (const Process *proc);
    void            OnProcessPreLoad    (const PeLoader *loader);
    void            OnProcessPostLoad   (const PeLoader *loader);
    void            OnWinapiPreCall     (Processor *cpu, uint apiIndex);
    void            OnWinapiPostCall    (Processor *cpu, uint apiIndex);

    Emulator *      GetEmulator() { return m_emulator; }
    ProDebugger *   GetDebugger() { return &m_debugger; }
    ProTracer *     GetTracer() { return &m_tracer; }
    Disassembler *  GetDisassembler() { return &m_disassembler; }
    TaintEngine *   GetTaintEngine() { return &m_taint; }
    Archive *       GetArchive() { return &m_archive; }
    ProPluginManager * GetPluginManager() { return &m_plugins; }
    Protocol *      GetProtocol() { return &m_protocol; }

    const Emulator *    GetEmulator() const { return m_emulator; }
    const ProDebugger * GetDebugger() const { return &m_debugger; }
    const ProTracer *   GetTracer() const { return &m_tracer; }
    const Disassembler *GetDisassembler() const { return &m_disassembler; }
    const TaintEngine * GetTaintEngine() const { return &m_taint; }
    const Archive *     GetArchive() const { return &m_archive; }
    const ProPluginManager* GetPluginManager() const { return &m_plugins; }
    const Protocol *    GetProtocol() const { return &m_protocol; }

    void            GetInstContext(InstContext *ctx) const;
    void            GetTraceContext(TraceContext *ctx, u32 eip) const;

    void            ReportBusy(bool isBusy);
    void            Terminate();
    
private:
    void            Intro() const;
    void            CreateArchiveDirectory();
    void            LoadArchive(const char *moduleName);

private:
    bool            m_enabled;
    Emulator *      m_emulator;

    ProDebugger     m_debugger;
    ProTracer       m_tracer;
    Disassembler    m_disassembler;
    TaintEngine     m_taint;
    ProphetFrame *  m_gui;
    Archive         m_archive;
    ProPluginManager    m_plugins;
    Protocol        m_protocol;

    //bool            m_skipDllEntries;
    //bool            m_mainEntryEntered;

    std::string     m_archivePath;
    bool            m_isArchiveLoaded;
};

#endif // __PROPHET_ENGINE_H__
