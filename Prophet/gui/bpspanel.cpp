#include "stdafx.h"
#include "bpspanel.h"
#include "engine.h"
#include "dbg/debugger.h"
#include "dbg/breakpoint.h"
#include "mainframe.h"
#include "cpupanel.h"

BreakpointsPanel::BreakpointsPanel( wxWindow *parent, ProphetFrame *dad )
    : SelectableScrolledControl(parent, wxSize(500, 100)), m_dad(dad)
{
    InitRender();
    InitMenu();
    m_engine    = NULL;
    m_debugger  = NULL;

    Bind(wxEVT_LEFT_DCLICK,     &BreakpointsPanel::OnLeftDoubleClick,   this, wxID_ANY);
    Bind(wxEVT_RIGHT_DOWN,      &BreakpointsPanel::OnRightDown,         this, wxID_ANY);
}

BreakpointsPanel::~BreakpointsPanel()
{
}

void BreakpointsPanel::UpdateData( ProEngine *eng )
{
    m_engine    = eng;
    m_debugger  = eng->GetDebugger();

    SetVirtualSize(m_width, m_debugger->GetNumBreakpoints() * m_lineHeight);
    Refresh();
}

void BreakpointsPanel::InitRender()
{
    wxFont f = wxFont(g_config.GetInt("BpsPanel", "FontSize", 9),
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

void BreakpointsPanel::InitMenu()
{
    m_popup = new wxMenu;
    m_popup->Append(ID_PopupShowCode, "&Show code");
    m_popup->Append(ID_PopupDelete, "&Delete");
    m_popup->Append(ID_PopupToggle, "&Toggle");

    Bind(wxEVT_COMMAND_MENU_SELECTED, &BreakpointsPanel::OnPopupShowCode,   this, ID_PopupShowCode);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &BreakpointsPanel::OnPopupDelete,     this, ID_PopupDelete);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &BreakpointsPanel::OnPopupToggle,     this, ID_PopupToggle);
}

void BreakpointsPanel::Draw( wxBufferedPaintDC &dc )
{
    wxPoint viewStart   = GetViewStart();
    wxSize clientSize   = GetClientSize();
    const int istart    = viewStart.y;
    const int iend      = istart + clientSize.GetHeight() / m_lineHeight;

    if (m_debugger != NULL && m_engine != NULL) {
        //SyncObjectLock lock(*m_archive);
        for (int i = istart; i <= min(m_debugger->GetNumBreakpoints()-1, iend); i++)
            DrawItem(dc, i);
    }
    
    dc.SetPen(*wxGREY_PEN);
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);

    int lineX = m_widthAddress + VertLineOffset;
    int lineY0 = viewStart.y * py, lineY1 = lineY0 + clientSize.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthModuleName;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDesc;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDisasm;
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

    const Breakpoint &bp = m_debugger->GetBreakpointIndex(index);//m_archive->Breakpoints[index];
    int w = 0;
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(bp.Enabled ? *wxGREEN_BRUSH : *wxRED_BRUSH);
    dc.DrawRectangle(w, h, m_widthAddress, m_lineHeight);
    dc.DrawText(wxString::Format("%08x", bp.Address), w, h);
    w += m_widthAddress;
    dc.DrawText(wxString::Format("%s", bp.ModuleName), w, h);
    w += m_widthModuleName;
    dc.DrawText(wxString::Format("%s", bp.Desc), w, h);
    w += m_widthDesc;
    const char *disasm = m_engine->GetDisassembler()->GetInst(bp.Address)->Main.CompleteInstr;
    dc.DrawText(wxString::Format("%s", disasm), w, h);
    w += m_widthDisasm;

}

void BreakpointsPanel::OnLeftDoubleClick( wxMouseEvent &event )
{
    if (!IsSelectedValid()) return;
    u32 eip = m_debugger->GetBreakpointIndex(m_currSelIndex).Address;
    m_dad->GetCpuPanel()->ShowCode(eip);
}

void BreakpointsPanel::OnPopupShowCode( wxCommandEvent &event )
{
    OnLeftDoubleClick(wxMouseEvent());
}

void BreakpointsPanel::OnPopupDelete( wxCommandEvent &event )
{
    if (!IsSelectedValid()) return;
    u32 eip = m_debugger->GetBreakpointIndex(m_currSelIndex).Address;
    m_engine->GetDebugger()->RemoveBreakpoint(eip);
    m_dad->GetCpuPanel()->Refresh();
    Refresh();
}

void BreakpointsPanel::OnPopupToggle( wxCommandEvent &event )
{
    if (!IsSelectedValid()) return;
    u32 eip = m_debugger->GetBreakpointIndex(m_currSelIndex).Address;
    m_engine->GetDebugger()->ToggleBreakpoint(eip);
    m_dad->GetCpuPanel()->Refresh();
    Refresh();
}

bool BreakpointsPanel::IsSelectedValid() const
{
    return m_currSelIndex >= 0 && m_currSelIndex < m_debugger->GetNumBreakpoints();
}

void BreakpointsPanel::OnRightDown( wxMouseEvent &event )
{
    OnLeftDown(event);
    OnLeftUp(event);
    PopupMenu(m_popup);
}
