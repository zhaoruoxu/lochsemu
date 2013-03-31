#include "stdafx.h"
#include "threadpanel.h"
#include "engine.h"
#include "dbg/debugger.h"
#include "emulator.h"


ThreadPanel::ThreadPanel( wxWindow *parent, ProphetFrame *dad )
    : SelectableScrolledControl(parent, wxSize(600, 200)), m_dad(dad)
{
    InitRender();

    m_debugger  = NULL;
    m_disasm    = NULL;
    m_count     = 0;
    m_currTid   = 0;

    Bind(wxEVT_LEFT_DCLICK,     &ThreadPanel::OnLeftDoubleClick,    this, wxID_ANY);
}

ThreadPanel::~ThreadPanel()
{

}

void ThreadPanel::UpdateData(ProEngine *engine)
{
    m_debugger  = engine->GetDebugger();
    m_disasm    = engine->GetDisassembler();
    m_count     = m_debugger->GetThreadCount();
    m_currTid   = m_debugger->GetCurrentThreadId();

    SetVirtualSize(m_width, m_lineHeight * m_count);
    Refresh();
}

void ThreadPanel::OnLeftDoubleClick( wxMouseEvent &event )
{
    int count = 0;
    int tid = 0;
    for (; tid < ProDebugger::MaxThreads; tid++) {
        if (m_debugger->GetThread(tid)) count++;
        if (count == m_currSelIndex+1) break;
    }
    if (m_debugger->GetThread(tid) == NULL) {
        wxMessageBox("Invalid thread selected! TID = " + tid);
        return;
    }
    m_debugger->SetCurrentThread(tid);
}

void ThreadPanel::InitRender()
{
    wxFont f = wxFont(g_config.GetInt("ThreadPanel", "FontSize", 9),
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("ThreadPanel", "FontName", "Lucida Console"));
    UpdateFont(f);

    m_widthIntId        = g_config.GetInt("ThreadPanel", "WidthIntID",  30);
    m_widthExtId        = g_config.GetInt("ThreadPanel", "WidthExtID",  50);
    m_widthStatus       = g_config.GetInt("ThreadPanel", "WidthStatus", 100);
    m_widthEip          = g_config.GetInt("ThreadPanel", "WidthEip",    75);
    m_widthModule       = g_config.GetInt("ThreadPanel", "WidthModule", 150);
    m_widthDisasm       = g_config.GetInt("ThreadPanel", "WidthDisasm", 300);
    m_width = m_widthIntId + m_widthExtId + m_widthStatus + m_widthEip + 
        m_widthModule + m_widthDisasm;

    m_currSelBrush      = wxBrush(wxColour(g_config.GetString("ThreadPanel", "CurrSelBrush", "#e0e0e0")));
    m_naBrush           = wxBrush(wxColour("#a0a0a0"));
    m_currThrBrush      = wxBrush(wxColour("#00ffff"));
}

void ThreadPanel::Draw( wxBufferedPaintDC &dc )
{
    if (m_debugger == NULL || m_disasm == NULL) return;

    wxPoint viewStart   = GetViewStart();
    wxSize clientSize   = GetClientSize();
    const int istart    = viewStart.y;
    const int iend      = istart + clientSize.GetHeight() / m_lineHeight;

    int index = 0;
    for (int i = istart; i <= ProDebugger::MaxThreads-1 && index <= iend; i++) {
        if (m_debugger->GetThread(i) != NULL)
            DrawItem(dc, index++);
    }

    dc.SetPen(*wxGREY_PEN);
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);

    int lineX = m_widthIntId + VertLineOffset;
    int lineY0 = viewStart.y * py, lineY1 = lineY0 + clientSize.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthExtId;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthStatus;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthEip;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthModule;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDisasm;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);

}

void ThreadPanel::DrawItem( wxBufferedPaintDC &dc, int index )
{
    Thread *t = m_debugger->GetThread(index);
    Assert(t);
    int h = m_lineHeight * index;
    wxRect rectToDraw(0, h, m_width, m_lineHeight);

    dc.SetPen(*wxTRANSPARENT_PEN);
    if (m_currSelIndex == index) {
        dc.SetBrush(m_currSelBrush);
        dc.DrawRectangle(rectToDraw);
    }

    if (t->IntID == m_currTid) {
        dc.SetBrush(m_currThrBrush);
        dc.DrawRectangle(0, h, m_widthIntId, m_lineHeight);
    }

    InstPtr pinst = m_disasm->GetInst(t->CPU(), t->CPU()->GetValidEip());
    uint nModule = t->CPU()->GetCurrentModule();
    const ModuleInfo *info = LxEmulator.Proc()->GetModuleInfo(nModule);

    int w = 0;
    dc.DrawText(wxString::Format("%d", t->IntID), w, h);
    w += m_widthIntId;
    dc.DrawText(wxString::Format("%x", t->ExtID), w, h);
    w += m_widthExtId;
    dc.DrawText(ProDebugger::StateText[m_debugger->GetState(index)], w, h);
    w += m_widthStatus;
    dc.DrawText(wxString::Format("%08x", t->CPU()->GetValidEip()), w, h);
    w += m_widthEip;
    dc.DrawText(info->Name, w, h);
    w += m_widthModule;
    if (pinst)
        dc.DrawText(pinst->Main.CompleteInstr, w, h);
}
