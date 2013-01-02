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
    //wxDECLARE_EVENT_TABLE();
    //DECLARE_EVENT_TABLE()
};

#endif // __STATUS_STATUSGUI_H__
