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
private:
    void    OnExit(wxCommandEvent &event);
    void    OnAbout(wxCommandEvent &event);
    void    OnClose(wxCloseEvent &event);

    /* View */
    void    OnSavePerspective(wxCommandEvent &event);
    void    OnLoadPerspective(wxCommandEvent &event);
    void    OnResetPerspective(wxCommandEvent &event);

    /* Debug */
    void    OnStepInto(wxCommandEvent &event);

    void    OnStatusTimer(wxTimerEvent &event);
private:
    void    InitMisc();
    void    InitUI();
    void    InitMenu();
    void    InitStatusBar();
private:
    ArietisEngine * m_engine;

    LogPanel *      m_logPanel;
    CpuPanel *      m_cpuPanel;

    wxString        m_defaultPerspective;
    wxAuiManager    m_auiManager;
    Config          m_config;

    wxTimer         m_statusTimer;
    wxStatusBar *   m_statusbar;
};
 
#endif // __ARIETIS_GUI_MAINFRAME_H__