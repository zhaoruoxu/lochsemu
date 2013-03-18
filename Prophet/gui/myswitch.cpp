#include "stdafx.h"
#include "myswitch.h"


MySwitch::MySwitch( wxWindow *parent, wxWindowID id, const wxString &label, const wxSize &size )
    : wxButton(parent, id, label, wxDefaultPosition, size, wxBORDER_NONE)
{
    m_colorInvalid  = wxColour("#808080");
    m_colorOn       = wxColour("#00ffff");
    m_colorOff      = wxColour("#ff0000");

    SetBackgroundColour(m_colorInvalid);
}

void MySwitch::SetOn( bool isOn )
{
    SetBackgroundColour(isOn ? m_colorOn : m_colorOff);
    Refresh();
}
