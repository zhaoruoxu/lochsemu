#pragma once

#ifndef __ARIETIS_ENGINE_H__
#define __ARIETIS_ENGINE_H__

#include "Arietis.h"
#include "debugger.h"
#include "gui/gui.h"
#include "static/disassembler.h"

struct InstContext {
    Disassembler::Inst  inst;
    u32 gp_regs[8];
};

class ArietisEngine {
public:
    ArietisEngine();
    ~ArietisEngine();

    void            Initialize();

    void            SetGuiFrame(ArietisFrame *frame);
    ArietisFrame *  GetGUI() const { Assert(m_gui); return m_gui; }

    void            OnPreExecute(Processor *cpu, const Instruction *inst);
    void            OnPostExecute(Processor *cpu, const Instruction *inst);
    void            OnProcessPreRun(const Process *proc, const Processor *cpu);

    Debugger *      GetDebugger() { return &m_debugger; }
    
    void            Log(const wxString &s);
private:
    Debugger        m_debugger;
    Disassembler    m_disassembler;
    ArietisFrame *  m_gui;
};

#endif // __ARIETIS_ENGINE_H__
