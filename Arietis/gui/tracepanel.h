#pragma once

#ifndef __ARIETIS_GUI_TRACEPANEL_H__
#define __ARIETIS_GUI_TRACEPANEL_H__

#include "Arietis.h"
#include "gui.h"
#include "tracer.h"
#include "custombase.h"

class TraceInfoPanel;
class TracePanel;


class CompositeTracePanel : public wxPanel {
    friend class TraceInfoPanel;
    friend class TracePanel;
public:
    CompositeTracePanel(wxWindow *parent, ContextPanel *ctxPanel);
    ~CompositeTracePanel();

    const wxFont *GetFont() const { return &m_font; }
    void        SetTracer(const ATracer *t) { m_tracer = t; }
    void        UpdateData();
    void        OnSelectionChange(int index);
private:
    void        Init();
private:
    const ATracer * m_tracer;
    TracePanel *    m_tracePanel;
    TraceInfoPanel *m_infoPanel;
    ContextPanel *  m_contextPanel;
    int             m_total;
};

class TraceInfoPanel : public wxPanel {
    friend class TracePanel;
public:
    TraceInfoPanel(CompositeTracePanel *parent);
    ~TraceInfoPanel() {}

    void        OnPaint(wxPaintEvent &event);
    void        OnEraseBackground(wxEraseEvent &event);
    void        UpdateData(const TraceContext &t);
    void        UpdateData();
private:
    void        Init();
private:
    CompositeTracePanel*    m_parent;
    TraceContext    m_trace;
    wxFont          m_font;
    wxBrush         m_bgBrush;        
    int             m_lineHeight;
};

class TracePanel : public SelectableScrolledControl {
    friend class TraceInfoPanel;
public:
    TracePanel(CompositeTracePanel *parent);
    ~TracePanel();

    void        UpdateData();
    void        OnSelectionChange() override;
    void        OnLeftDoubleClick(wxMouseEvent &event);
    const static int VertLineOffset = -2;
private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawTrace(wxBufferedPaintDC &dc, const TraceContext &trace, int index);
private:
    CompositeTracePanel *   m_parent;

    wxPen       m_currPen;
    wxPen       m_currSelPen;
    int         m_widthIp;
    int         m_widthDisasm;
    int         m_widthTaint;
    int         m_width;
};

#endif // __ARIETIS_GUI_TRACEPANEL_H__
