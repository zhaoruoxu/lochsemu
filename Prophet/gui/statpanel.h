#pragma once
 
#ifndef __PROPHET_GUI_STATPANEL_H__
#define __PROPHET_GUI_STATPANEL_H__
 
#include "prophet.h"
#include "gui.h"
#include "engine.h"

class StatPanel : public wxPanel {
public:
    StatPanel(wxWindow *parent, ProphetFrame *dad, ProEngine *engine);
    ~StatPanel();

    void        Start();
    void        Stop();
    void        OnPaint(wxPaintEvent &event);
    void        OnEraseBackground(wxEraseEvent& event);
    void        OnTimer(wxTimerEvent &event);
    void        OnResize(wxSizeEvent &event);
    void        ReportBusy(bool isbusy);
private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc);

private:
    ProphetFrame *  m_dad;
    long        m_startTime;
    bool        m_started;
    wxFont      m_font;
    ProEngine * m_engine;
    int         m_lineHeight;
    wxTimer     m_timer;
    bool        m_isbusy;
    int         m_interval;
    wxPen       m_midLinePen;
};
 
#endif // __PROPHET_GUI_STATPANEL_H__