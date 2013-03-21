#pragma once
 
#ifndef __PROPHET_GUI_BPSPANEL_H__
#define __PROPHET_GUI_BPSPANEL_H__
 
#include "prophet.h"
#include "gui.h"
#include "custombase.h"

#include "parallel.h"

class BreakpointsPanel : public SelectableScrolledControl {
public:
    BreakpointsPanel(wxWindow *parent, ProphetFrame *dad);
    ~BreakpointsPanel();

    void        UpdateData(ProEngine *eng);
    void        OnLeftDoubleClick(wxMouseEvent &event);
    void        OnRightDown(wxMouseEvent &event);
    void        OnPopupShowCode(wxCommandEvent &event);
    void        OnPopupDelete(wxCommandEvent &event);
    void        OnPopupToggle(wxCommandEvent &event);
private:
    void        InitRender();
    void        InitMenu();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawItem(wxBufferedPaintDC &dc, int index);
private:
    bool        IsSelectedValid() const;
private:
    wxMenu *    m_popup;
    ProphetFrame *  m_dad;

    ProEngine *   m_engine;
    ProDebugger * m_debugger;

    int         m_widthAddress;
    int         m_widthModuleName;
    int         m_widthDisasm;
    int         m_widthDesc;
    int         m_width;

    wxBrush     m_currSelBrush;
};

#endif // __PROPHET_GUI_BPSPANEL_H__