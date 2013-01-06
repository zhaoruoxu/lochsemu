#include "stdafx.h"
#include "tracepanel.h"


TracePanel::TracePanel( CompositeTracePanel *parent )
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(600, 200),
    wxVSCROLL | wxHSCROLL), m_parent(parent)
{
    InitRender();

    Bind(wxEVT_PAINT, &TracePanel::OnPaint, this, wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &TracePanel::OnEraseBackGround, this, wxID_ANY);
    
}

TracePanel::~TracePanel()
{

}

void TracePanel::InitRender()
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_font =    wxFont(g_config.GetInt("TracePanel", "FontSize", 8), 
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("TracePanel", "FontName", "Lucida Console"));
    m_currPen       = wxPen(wxColour(g_config.GetString("TracePanel", "CurrentColor", "#0080ff")));
    m_currSelPen    = wxPen(wxColour(g_config.GetString("TracePanel", "CurrentSelectionColor", "#808080")));
    m_bgBrush       = wxBrush(wxColour(g_config.GetString("TracePanel", "BgColor", "#e0e0ff")));
    m_widthIp       = g_config.GetInt("TracePanel", "WidthIp", 70);
    m_widthDisasm   = g_config.GetInt("TracePanel", "WidthDisasm", 300);
    m_width         = m_widthIp + m_widthDisasm;

    SetFont(m_font);
    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_lineHeight    = dc.GetFontMetrics().height;
    SetScrollRate(1, m_lineHeight);
}

void TracePanel::OnPaint( wxPaintEvent& event )
{
    wxBufferedPaintDC dc(this);
    DoPrepareDC(dc);
    dc.SetBackground(m_bgBrush);
    dc.Clear();

    if (m_parent->m_tracer == NULL) return;

    Draw(dc);
}

void TracePanel::OnEraseBackGround( wxEraseEvent& event )
{

}

void TracePanel::Draw( wxBufferedPaintDC &dc )
{
    m_parent->m_tracer->Lock();

    const ATracer::TraceVec &vec = m_parent->m_tracer->GetData();
    const int N = vec.size();
    SetVirtualSize(m_width, m_lineHeight * (N ));
    m_parent->m_total = N;

    wxPoint viewStart   = GetViewStart();
    wxSize clientSize   = GetClientSize();
    const int istart    = viewStart.y;
    const int iend      = istart + clientSize.GetHeight() / m_lineHeight;

    dc.SetBrush(m_bgBrush);
    for (int i = istart; i <= min(N-1, iend); i++) {
        DrawTrace(dc, vec[i], i);
    }

    dc.SetPen(*wxGREY_PEN);
    const static int VertLineOffset = -2;
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);
    int lineX = m_widthIp + VertLineOffset;
    int lineY0 = viewStart.y * py, lineY1 = lineY0 + clientSize.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDisasm;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);

    m_parent->m_tracer->Unlock();
}

void TracePanel::DrawTrace( wxBufferedPaintDC &dc, const ATracer::Trace &trace, int index )
{
    int h = m_lineHeight * index;
    wxRect rectToDraw(0, h, m_width, m_lineHeight);

    if (index == m_parent->m_total - 1) {
        dc.SetPen(m_currPen);
        dc.DrawRectangle(rectToDraw);
    }
    dc.SetPen(*wxBLACK_PEN);

    dc.DrawText(wxString::Format("%08X", trace.inst.eip), 0, h);
    dc.DrawText(trace.inst.ptr->Main.CompleteInstr, m_widthIp, h);
}

void TracePanel::UpdateData()
{
    Refresh();
    Update();
    Scroll(0, m_parent->m_total);
}


TraceInfoPanel::TraceInfoPanel( CompositeTracePanel *parent )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 30)), m_parent(parent)
{
    Init();

    Bind(wxEVT_PAINT, &TraceInfoPanel::OnPaint, this, wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &TraceInfoPanel::OnEraseBackground, this, wxID_ANY);
}

void TraceInfoPanel::Init()
{
    m_font =    wxFont(g_config.GetInt("TraceInfoPanel", "FontSize", 8), 
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("TracePanel", "FontName", "Lucida Console"));

    m_bgBrush   = wxBrush(wxColour(g_config.GetString("TraceInfoPanel", "BgColor", "#b0b0b0")));

    SetFont(m_font);

    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_lineHeight = dc.GetFontMetrics().height;
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void TraceInfoPanel::OnPaint( wxPaintEvent &event )
{
    wxBufferedPaintDC dc(this);
    dc.SetBackground(m_bgBrush);
    dc.Clear();
    dc.DrawText(wxString::Format("Total: %d", m_parent->m_total), 0, 0);
}

void TraceInfoPanel::OnEraseBackground( wxEraseEvent &event )
{

}

void TraceInfoPanel::UpdateData()
{
    Refresh();
    Update();
}

CompositeTracePanel::CompositeTracePanel( wxWindow *parent )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 400))
{
    Init();
    m_tracer = NULL;
}

CompositeTracePanel::~CompositeTracePanel()
{

}

void CompositeTracePanel::Init()
{
//     m_font =    wxFont(g_config.GetInt("TracePanel", "FontSize", 8), 
//         wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
//         g_config.GetString("TracePanel", "FontName", "Lucida Console"));

    m_tracePanel    = new TracePanel(this);
    m_infoPanel     = new TraceInfoPanel(this);
    wxBoxSizer * vsizer = new wxBoxSizer(wxVERTICAL);
    vsizer->Add(m_tracePanel, 1, wxEXPAND | wxALL);
    vsizer->Add(m_infoPanel, 0, wxEXPAND | wxALL);
    SetSizer(vsizer);
}

void CompositeTracePanel::UpdateData()
{
    m_tracePanel->UpdateData();
    m_infoPanel->UpdateData();
}

