#pragma once
 
#ifndef __PROPHET_GUI_THREADPANEL_H__
#define __PROPHET_GUI_THREADPANEL_H__
 
#include "prophet.h"
#include "gui.h"
#include "custombase.h"

class ThreadPanel : public SelectableScrolledControl {
public:
    ThreadPanel(wxWindow *parent, ProphetFrame *dad);
    ~ThreadPanel();

    void        UpdateData(ProEngine *engine);
    void        OnLeftDoubleClick(wxMouseEvent &event);

private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawItem(wxBufferedPaintDC &dc, int index);
private:
    ProphetFrame *  m_dad;

    ProDebugger *   m_debugger;
    Disassembler *  m_disasm;

    int             m_widthIntId;
    int             m_widthExtId;
    int             m_widthStatus;
    int             m_widthEip;
    int             m_widthModule;
    int             m_widthDisasm;
    int             m_width;
    
    int             m_count;
    int             m_currTid;

    wxBrush         m_currSelBrush;
    wxBrush         m_naBrush;
    wxBrush         m_currThrBrush;
};
 
#endif // __PROPHET_GUI_THREADPANEL_H__