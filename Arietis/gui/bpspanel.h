#pragma once
 
#ifndef __ARIETIS_GUI_BPSPANEL_H__
#define __ARIETIS_GUI_BPSPANEL_H__
 
#include "Arietis.h"
#include "gui.h"
#include "custombase.h"

#include "parallel.h"

class BreakpointsPanel : public SelectableScrolledControl {
public:
    BreakpointsPanel(wxWindow *parent);
    ~BreakpointsPanel();

    void        UpdateData(const AEngine *eng);

private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawItem(wxBufferedPaintDC &dc, int index);

private:
    const AEngine * m_engine;
    const Archive * m_archive;

    int         m_widthAddress;
    int         m_widthModuleName;
    int         m_widthDisasm;
    int         m_widthDesc;
    int         m_width;

    wxBrush     m_currSelBrush;
    //MutexCS     m_mutex;
};

#endif // __ARIETIS_GUI_BPSPANEL_H__