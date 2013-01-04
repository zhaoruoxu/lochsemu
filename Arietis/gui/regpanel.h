#pragma once

#ifndef __ARIETIS_GUI_REGPANEL_H__
#define __ARIETIS_GUI_REGPANEL_H__

#include "Arietis.h"
#include "gui.h"

class RegPanel : public wxPanel {
public:
    RegPanel(wxWindow *parent);

    void        SetValues(const Processor *cpu);
private:
    void        InitUI();
private:
    static const int RegCount = 8;
    static const wxString RegLabels[];
    struct RegControl {
        wxStaticText *Label;
        wxStaticText *Value;
    } m_regs[RegCount];

    wxStaticText *  m_eipLabel;
    wxStaticText *  m_eipValue;

    wxFont          m_font;
};

#endif // __ARIETIS_GUI_REGPANEL_H__
