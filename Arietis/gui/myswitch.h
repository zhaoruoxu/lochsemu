#pragma once
 
#ifndef __ARIETIS_GUI_SWITCH_H__
#define __ARIETIS_GUI_SWITCH_H__
 
#include "Arietis.h"
#include "gui.h"


class MySwitch : public wxButton {
public:
    MySwitch(wxWindow *parent, wxWindowID id, const wxString &label, const wxSize &size);

    void        SetOn(bool isOn);
private:
    wxColour    m_colorOn;
    wxColour    m_colorOff;
    wxColour    m_colorInvalid;
};
 
#endif // __ARIETIS_GUI_SWITCH_H__