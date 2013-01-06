#pragma once

#ifndef __ARIETIS_GUI_TRACEPANEL_H__
#define __ARIETIS_GUI_TRACEPANEL_H__

#include "Arietis.h"
#include "gui.h"
#include "tracer.h"

class TraceInfoPanel;
class TracePanel;


class CompositeTracePanel : public wxPanel {
    friend class TraceInfoPanel;
    friend class TracePanel;
public:
    CompositeTracePanel(wxWindow *parent);
    ~CompositeTracePanel();

    const wxFont *GetFont() const { return &m_font; }
    void        SetTracer(const ATracer *t) { m_tracer = t; }
    void        UpdateData();
private:
    void        Init();
private:
    const ATracer * m_tracer;
    //wxFont          m_font;
    TracePanel *    m_tracePanel;
    TraceInfoPanel *m_infoPanel;
    int             m_total;
};

class TraceInfoPanel : public wxPanel {
public:
    TraceInfoPanel(CompositeTracePanel *parent);
    ~TraceInfoPanel() {}

    void        OnPaint(wxPaintEvent &event);
    void        OnEraseBackground(wxEraseEvent &event);
    void        UpdateData();
private:
    void        Init();
private:
    CompositeTracePanel*    m_parent;
    wxFont          m_font;
    wxBrush         m_bgBrush;        
    int             m_lineHeight;
};

class TracePanel : public wxScrolledWindow {
public:
    TracePanel(CompositeTracePanel *parent);
    ~TracePanel();

    //void        SetTracer(const ATracer *tracer) { m_tracer = tracer; }

    void        OnPaint(wxPaintEvent &event);
    void        OnEraseBackGround(wxEraseEvent &event);
    void        UpdateData();

private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc);
    void        DrawTrace(wxBufferedPaintDC &dc, const ATracer::Trace &trace, int index);
private:
    //const ATracer *     m_tracer;
    CompositeTracePanel *   m_parent;

    wxPen       m_currPen;
    wxPen       m_currSelPen;
    wxBrush     m_bgBrush;
    wxFont      m_font;
    int         m_widthIp;
    int         m_widthDisasm;
    int         m_width;
    int         m_lineHeight;
};

#endif // __ARIETIS_GUI_TRACEPANEL_H__
