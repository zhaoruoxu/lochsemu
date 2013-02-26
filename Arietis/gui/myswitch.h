#pragma once
 
#ifndef __ARIETIS_GUI_SWITCH_H__
#define __ARIETIS_GUI_SWITCH_H__
 
#include "Arietis.h"
#include "gui.h"


class MySwitch : public wxButton {
public:
    MySwitch(wxWindow *parent, wxWindowID id, const wxString &label, const wxSize &size);

    void        SetOn(bool isOn);
};
 
#endif // __ARIETIS_GUI_SWITCH_H__