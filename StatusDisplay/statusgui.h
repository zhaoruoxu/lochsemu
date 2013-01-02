#pragma once

#ifndef __STATUS_STATUSGUI_H__
#define __STATUS_STATUSGUI_H__

#include "StatusDisplay.h"

class StatusDisplay : public wxApp {
public:
    virtual bool        OnInit();
};

class StatusDisplayFrame: public wxFrame
{
public:
    StatusDisplayFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnTextChange(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
private:
    void InitUI();
    void InitMenus();
    void InitControls();
    void UpdateControls();
private:
    int             m_updateInterval;
    wxStaticText *  m_textCpuUsage;
    wxStaticText *  m_textMemUsage;
    wxStaticText *  m_textThreads;
    wxTimer         m_timer;
    wxButton *      m_buttonApply;
    wxTextCtrl *    m_textInterval;
};

#endif // __STATUS_STATUSGUI_H__
