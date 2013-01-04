#pragma once
 
#ifndef __ARIETIS_GUI_LOGPANEL_H__
#define __ARIETIS_GUI_LOGPANEL_H__
 
#include "Arietis.h"
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
 
#endif // __ARIETIS_GUI_LOGPANEL_H__