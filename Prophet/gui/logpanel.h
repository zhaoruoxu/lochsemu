#pragma once
 
#ifndef __PROPHET_GUI_LOGPANEL_H__
#define __PROPHET_GUI_LOGPANEL_H__
 
#include "Prophet.h"
#include "gui.h"

class LogPanel : public wxPanel {
public:
    LogPanel(wxWindow *parent);

    void        Log(const wxString &s);

private:
    void        InitUI();

public:
    void        OnClear(wxCommandEvent &event);

private:
    wxTextCtrl  *m_log;
};
 
#endif // __PROPHET_GUI_LOGPANEL_H__