#pragma once
 
#ifndef __ARIETIS_GUI_MAINFRAME_H__
#define __ARIETIS_GUI_MAINFRAME_H__
 
#include "Arietis.h"
#include "gui.h"

class ArietisFrame : public wxFrame {
public:
    ArietisFrame(ArietisEngine *engine);
    ~ArietisFrame();


    void    DebugLog(const wxString &s);
    void    DebugStepCallback(const Processor *cpu, const Instruction *inst);

    CpuPanel *  GetCpuPanel() const { return m_cpuPanel; }
    ContextPanel *  GetRegPanel() const { return m_contextPanel; }
    LogPanel *  GetLogPanel() const { return m_logPanel; }
private:
    void    OnExit(wxCommandEvent &event);
    void    OnAbout(wxCommandEvent &event);
    void    OnClose(wxCloseEvent &event);

    /* View */
    void    OnSavePerspective(wxCommandEvent &event);
    void    OnLoadPerspective(wxCommandEvent &event);
    void    OnResetPerspective(wxCommandEvent &event);

    /* Debug */
    void    OnRun(wxCommandEvent &event);
    void    OnStepOver(wxCommandEvent &event);
    void    OnStepInto(wxCommandEvent &event);
    void    OnStepOut(wxCommandEvent &event);
    void    OnToggleBreakpoint(wxCommandEvent &event);

    void    OnStatusTimer(wxTimerEvent &event);
private:
    void    InitMisc();
    void    InitUI();
    void    InitMenu();
    void    InitStatusBar();
    void    InitToolbars();
private:
    ArietisEngine * m_engine;

    LogPanel *      m_logPanel;
    CpuPanel *      m_cpuPanel;
    ContextPanel *  m_contextPanel;

    wxString        m_defaultPerspective;
    wxAuiManager    m_auiManager;
    Config          m_config;

    wxTimer         m_statusTimer;
    wxStatusBar *   m_statusbar;
};
 
#endif // __ARIETIS_GUI_MAINFRAME_H__