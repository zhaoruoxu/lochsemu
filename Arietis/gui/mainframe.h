#pragma once
 
#ifndef __ARIETIS_GUI_MAINFRAME_H__
#define __ARIETIS_GUI_MAINFRAME_H__
 
#include "Arietis.h"
#include "gui.h"

class ArietisFrame : public wxFrame {
public:
    ArietisFrame(AEngine *engine);
    ~ArietisFrame();


    void    PreExecSingleStepCallback(const Processor *cpu, const Instruction *inst);

    CpuPanel *              GetCpuPanel() const { return m_cpuPanel; }
    ContextPanel *          GetRegPanel() const { return m_contextPanel; }
    CompositeTracePanel *   GetTracePanel() const { return m_tracePanel; }
    MemInfoPanel *          GetMemInfoPanel() const { return m_memInfoPanel; }
    void    ReportBusy(bool isBusy);
    void    OnProcessLoaded(LPCSTR path);
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
    AEngine * m_engine;

    CpuPanel *      m_cpuPanel;
    ContextPanel *  m_contextPanel;
    CompositeTracePanel *    m_tracePanel;
    MemInfoPanel *  m_memInfoPanel;
    MemDataPanel *  m_memDataPanel;

    wxString        m_defaultPerspective;
    wxAuiManager    m_auiManager;

    wxTimer         m_statusTimer;
    wxStatusBar *   m_statusbar;
};
 
#endif // __ARIETIS_GUI_MAINFRAME_H__