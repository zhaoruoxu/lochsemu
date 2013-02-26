#include "stdafx.h"
#include "bpspanel.h"
#include "archive.h"
#include "engine.h"

BreakpointsPanel::BreakpointsPanel( wxWindow *parent )
    : SelectableScrolledControl(parent, wxSize(500, 100))
{
    InitRender();
    m_engine    = NULL;
    m_archive   = NULL;
}

BreakpointsPanel::~BreakpointsPanel()
{
}

void BreakpointsPanel::UpdateData( const AEngine *eng )
{
    m_engine    = eng;
    m_archive   = eng->GetArchive();

    SetVirtualSize(m_width, m_archive->Breakpoints.size() * m_lineHeight);
    Refresh();
}

void BreakpointsPanel::InitRender()
{
    wxFont f = wxFont(g_config.GetInt("BpsPanel", "FontSize", 8),
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false,
        g_config.GetString("BpsPanel", "FontName", "Lucida Console"));
    UpdateFont(f);

    m_widthAddress      = g_config.GetInt("BpsPanel", "WidthAddress", 75);
    m_widthModuleName   = g_config.GetInt("BpsPanel", "WidthModuleName", 100);
    m_widthDisasm       = g_config.GetInt("BpsPanel", "WidthDisasm", 300);
    m_widthDesc         = g_config.GetInt("BpsPanel", "WdithDesc", 120);
    m_width             = m_widthAddress + m_widthModuleName + m_widthDisasm + m_widthDesc;

    m_currSelBrush      = wxBrush(wxColour(g_config.GetString("BpsPanel", "CurrSelBrush", "#e0e0e0")));
}

void BreakpointsPanel::Draw( wxBufferedPaintDC &dc )
{
    wxPoint viewStart   = GetViewStart();
    wxSize clientSize   = GetClientSize();
    const int istart    = viewStart.y;
    const int iend      = istart + clientSize.GetHeight() / m_lineHeight;

    if (m_archive != NULL && m_engine != NULL) {
        SyncObjectLock lock(*m_archive);
        for (int i = istart; i <= min((int) m_archive->Breakpoints.size()-1, iend); i++)
            DrawItem(dc, i);
    }

    dc.SetPen(*wxBLUE_PEN);
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);

    int lineX = m_widthAddress + VertLineOffset;
    int lineY0 = viewStart.y * py, lineY1 = lineY0 + clientSize.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthModuleName;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDisasm;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDesc;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
}

void BreakpointsPanel::DrawItem( wxBufferedPaintDC &dc, int index )
{
    int h = m_lineHeight * index;
    wxRect rectToDraw(0, h, m_width, m_lineHeight);
    if (index == m_currSelIndex) {
        dc.SetBrush(m_currSelBrush);
        dc.DrawRectangle(rectToDraw);
    }

    const Breakpoint &bp = m_archive->Breakpoints[index];
    int w = 0;
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(bp.Enabled ? *wxGREEN_BRUSH : *wxRED_BRUSH);
    dc.DrawRectangle(w, h, m_widthAddress, m_lineHeight);
    dc.DrawText(wxString::Format("%08x", bp.Address), w, h);
    w += m_widthAddress;
    dc.DrawText(wxString::Format("%s", bp.ModuleName), w, h);
    w += m_widthModuleName;
    const char *disasm = m_engine->GetDisassembler()->GetInst(bp.Address)->Main.CompleteInstr;
    dc.DrawText(wxString::Format("%s", disasm), w, h);
    w += m_widthDisasm;
    dc.DrawText(wxString::Format("%s", bp.Desc), w, h);
    w += m_widthDesc;
}
