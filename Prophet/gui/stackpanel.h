#pragma once
 
#ifndef __PROPHET_GUI_CALLSTACKPANEL_H__
#define __PROPHET_GUI_CALLSTACKPANEL_H__

#include "prophet.h"
#include "gui.h"
#include "custombase.h"

struct StackCallInfo {
    wxString    Module;
    wxString    Func;
};

class StackPanel : public SelectableScrolledControl {
public:
    StackPanel(wxWindow *parent, ProphetFrame *dad, ProEngine *engine);
    ~StackPanel();

    void        UpdateData(const Processor *cpu);

    void        OnLeftDoubleClick(wxMouseEvent &event);
private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawItem(wxBufferedPaintDC &dc, Stack *s, int index);

private:
    ProEngine * m_engine;
    ProphetFrame *  m_dad;
    const Processor * m_cpu;
    int         m_stackSize;
    
    int         m_widthAddr;
    int         m_widthVal;
    int         m_widthDesc;
    int         m_width;
    
    wxBrush     m_currSelBrush;

    bool        m_isPrevCall;
    u32         m_prevEsp;

    //std::unordered_map<u32, StackCallInfo>  m_callinfo;
};


#endif // __PROPHET_GUI_CALLSTACKPANEL_H__