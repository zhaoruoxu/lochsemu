#pragma once

#ifndef __ARIETIS_ENGINE_H__
#define __ARIETIS_ENGINE_H__

#include "Arietis.h"
#include "debugger.h"
#include "gui/gui.h"
#include "static/disassembler.h"

class ArietisEngine {
public:
    ArietisEngine();
    ~ArietisEngine();

    void            Initialize();

    void            SetGuiFrame(ArietisFrame *frame);
    ArietisFrame *  GetGUI() const { Assert(m_gui); return m_gui; }

    void            OnPreExecute(Processor *cpu, const Instruction *inst);
    void            OnPostExecute(Processor *cpu, const Instruction *inst);

    /* Debugger */
    void            OnStepInto();

    void            Log(const wxString &s);
private:
    Debugger        m_debugger;
    //Disassembler    m_disassembler;
    ArietisFrame *  m_gui;
};

#endif // __ARIETIS_ENGINE_H__
