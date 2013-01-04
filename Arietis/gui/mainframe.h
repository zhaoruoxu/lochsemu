#pragma once
 
#ifndef __ARIETIS_GUI_MAINFRAME_H__
#define __ARIETIS_GUI_MAINFRAME_H__
 
#include "Arietis.h"

class ArietisFrame : public wxFrame {
public:
    ArietisFrame();
    ~ArietisFrame();

private:
    void    OnExit(wxCommandEvent &event);
    void    OnAbout(wxCommandEvent &event);
    void    OnClose(wxCloseEvent &event);
    void    OnSavePerspective(wxCommandEvent &event);
    void    OnLoadPerspective(wxCommandEvent &event);
    void    OnResetPerspective(wxCommandEvent &event);
    void    OnStatusTimer(wxTimerEvent &event);
private:
    void    InitMisc();
    void    InitUI();
    void    InitMenu();
    void    InitStatusBar();
private:
    wxString        m_defaultPerspective;
    wxAuiManager    m_auiManager;
    Config          m_config;

    wxTimer         m_statusTimer;
    wxStatusBar *   m_statusbar;
};
 
#endif // __ARIETIS_GUI_MAINFRAME_H__