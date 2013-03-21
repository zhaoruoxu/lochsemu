#include "stdafx.h"
#include "statpanel.h"

StatPanel::StatPanel( wxWindow *parent, ProphetFrame *dad, ProEngine *engine )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 300)),
    m_timer(this, ID_StatTimer), m_engine(engine), m_dad(dad)
{
    InitRender();

    Bind(wxEVT_PAINT, &StatPanel::OnPaint, this, wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &StatPanel::OnEraseBackground, this, wxID_ANY);
    Bind(wxEVT_SIZE, &StatPanel::OnResize, this, wxID_ANY);

    m_interval = g_config.GetInt("StatPanel", "UpdateInterval", 40);
    m_timer.Start(m_interval);
    Bind(wxEVT_TIMER, &StatPanel::OnTimer, this, ID_StatTimer);
    m_isbusy = false;
    m_started = false;
}

StatPanel::~StatPanel()
{
}


void StatPanel::Start()
{
    m_started = true;
    m_startTime = wxGetLocalTime();
    Refresh();
}

void StatPanel::Stop()
{
    m_started = false;
    m_timer.Stop();
}

void StatPanel::ReportBusy( bool isbusy )
{
    m_isbusy = isbusy;
    Refresh();
}

void StatPanel::InitRender()
{
    m_font = wxFont(g_config.GetInt("StatPanel", "FontSize", 9), 
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("StatPanel", "FontName", "Lucida Console"));
    SetFont(m_font);

    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_lineHeight = dc.GetFontMetrics().height;

    m_midLinePen = wxPen(wxColour("#c0c0c0"), 1, wxPENSTYLE_DOT_DASH);

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void StatPanel::OnPaint( wxPaintEvent &event )
{
    wxBufferedPaintDC dc(this);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    Draw(dc);
}

void StatPanel::OnEraseBackground( wxEraseEvent& event )
{

}

static INLINE int GetY(int ystart, int height, uint x, uint level)
{
    return height - 2 - (height - ystart) * x / level;
}

static INLINE int GetX(int xstart, int width, int i, int total)
{
    return xstart + (width - xstart) * i / total;
}

void StatPanel::Draw( wxBufferedPaintDC &dc )
{
    if (!m_started) return;

    static const uint Levels[] = {
        1000,  10000, 100000, 200000, 500000, 1000000, 2000000, 5000000, 10000000, 100000000, 0xffffffff
    };
    static const wxString LevelText[] = {
        "1k",  "10k", "100k", "200k", "500k",  "1M",   "2M",    "5M",    "10M",    "100M",     "Inf."
    };
    static const int CeilingBanner   = 15;
    static const int LeftBanner     = 50;

    wxSize s = GetSize();
    wxRect rectText(0, 0, s.GetWidth(), CeilingBanner);
    if (m_isbusy) {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(*wxRED_BRUSH);
        dc.DrawRectangle(rectText);
        dc.SetTextForeground(*wxWHITE);
    } else {
        dc.SetTextForeground(*wxBLACK);
    }
    long minute = (wxGetLocalTime() - m_startTime) / 60;
    long second = (wxGetLocalTime() - m_startTime) % 60;
    dc.DrawText(wxString::Format(" %02d:%02d %s", minute, second, m_isbusy ? "BUSY" : "IDLE"),
        0, 1);

    dc.SetTextForeground(*wxBLACK);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(0, CeilingBanner, s.GetWidth(), CeilingBanner);

    Statistics *stat = m_engine->GetStatistics();
    uint m = stat->GetMax() * 1000 / m_interval;
    int level = 0;
    while (level < _countof(Levels)) {
        if (m <= Levels[level]) break;
        level++;
    }

    // left banner
    dc.DrawText(LevelText[level], 0, CeilingBanner+2);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(LeftBanner, CeilingBanner, LeftBanner, s.GetHeight());

    // chart
    int total = stat->GetTotal();
    const int xstart = LeftBanner;
    const int ystart = CeilingBanner;
    uint lastVal = stat->Get(0) * 1000 / m_interval;

    dc.SetPen(m_midLinePen);
    for (int i = 1; i <= 3; i++) {
        const int y = GetY(ystart, s.GetHeight(), i, 4);
        dc.DrawLine(xstart, y, s.GetWidth(), y);
    }
    dc.SetPen(*wxBLUE_PEN);
    for (int i = 1; i < total; i++) {
        uint val = stat->Get(i) * 1000 / m_interval;
        int x0 = GetX(xstart, s.GetWidth(), i-1, total-1);
        int y0 = GetY(ystart, s.GetHeight(), lastVal, Levels[level]);
        int x1 = GetX(xstart, s.GetWidth(), i, total-1);
        int y1 = GetY(ystart, s.GetHeight(), val, Levels[level]);
        dc.DrawLine(x0, y0, x1, y1);
        lastVal = val;
    }
}

void StatPanel::OnTimer( wxTimerEvent &event )
{
    if (!m_started) return;
    Refresh();
    m_engine->GetStatistics()->Advance();
}

void StatPanel::OnResize( wxSizeEvent &event )
{
    //wxSize s = event.GetSize();
    //LxInfo("width = %d, height = %d\n", s.GetWidth(), s.GetHeight());
    Refresh();
}



