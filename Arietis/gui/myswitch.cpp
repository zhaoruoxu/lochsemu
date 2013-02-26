#include "stdafx.h"
#include "myswitch.h"


MySwitch::MySwitch( wxWindow *parent, wxWindowID id, const wxString &label, const wxSize &size )
    : wxButton(parent, id, label, wxDefaultPosition, size, wxBORDER_NONE)
{

}

void MySwitch::SetOn( bool isOn )
{
    SetBackgroundColour(isOn ? *wxGREEN : *wxRED);
}
