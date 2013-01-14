#include "stdafx.h"
#include "mempanel.h"
#include "common/parallel.h"
#include "core/memory.h"
#include "core/emulator.h"
#include "core/process.h"

static bool SecCmp(const SectionContext &l, const SectionContext &r)
{
    return l.Base < r.Base;
}

SectionContext::SectionContext( const SectionInfo &sec, const ModuleInfo *minfo )
{
    Base    = sec.base;
    Size    = sec.size;
    Desc    = sec.Desc;
    ModuleImageBase = minfo->ImageBase;
    ModuleName      = minfo->Name;
}

MemInfoPanel::MemInfoPanel(wxWindow *parent)
    : SelectableScrolledControl(parent, wxSize(400, 200))
{
    m_mutex = MutexCS::Create();
    InitRender();

    Bind(wxEVT_LEFT_DCLICK,     &MemInfoPanel::OnLeftDoubleClick, this, wxID_ANY);
}

MemInfoPanel::~MemInfoPanel()
{
    MutexCS::Destroy(m_mutex);
}

void MemInfoPanel::UpdateData( const Emulator *emu, const Memory *mem )
{
    {
        MutexCSLock lock(*m_mutex);
        std::vector<SectionInfo>    secInfo = mem->GetMemoryInfo();
        const Process *proc = emu->Proc();

        m_data.clear();
        for (auto &sec : secInfo)
            m_data.emplace_back(sec, proc->GetModuleInfo(sec.Module));
        std::sort(m_data.begin(), m_data.end(), SecCmp);
    }
    
    SetVirtualSize(m_width, m_data.size() * m_lineHeight);
    Refresh();
}

void MemInfoPanel::InitRender()
{
    wxFont f = wxFont(g_config.GetInt("MemInfoPanel", "FontSize", 8),
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false,
        g_config.GetString("MemInfoPanel", "FontName", "Lucida Console"));
    UpdateFont(f);

    m_widthRange    = g_config.GetInt("MemInfoPanel", "WidthRange", 130);
    m_widthSize     = g_config.GetInt("MemInfoPanel", "WidthSize", 70);
    m_widthModule   = g_config.GetInt("MemInfoPanel", "WidthModule", 70);
    m_widthDesc     = g_config.GetInt("MemInfoPanel", "WidthDesc", 200);
    m_width = m_widthRange + m_widthSize + m_widthModule + m_widthDesc;

    m_currSelBrush  = wxBrush(wxColour(g_config.GetString("MemInfoPanel", "CurrSelColor", "#00c0ff")));
}

void MemInfoPanel::Draw( wxBufferedPaintDC &dc )
{
    static const int VertLineOffset = -2;
    MutexCSLock lock(*m_mutex);
    
    wxPoint viewStart   = GetViewStart();
    wxSize clientSize   = GetClientSize();
    const int istart    = viewStart.y;
    const int iend      = istart + clientSize.GetHeight() / m_lineHeight;

    for (int i = istart; i <= min((int) m_data.size()-1, iend); i++) {
        DrawItem(dc, i);
    }

    dc.SetPen(*wxBLUE_PEN);
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);

    int lineX = m_widthRange + VertLineOffset;
    int lineY0 = viewStart.y * py, lineY1 = lineY0 + clientSize.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthSize;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthModule;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDesc;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
}

void MemInfoPanel::DrawItem( wxBufferedPaintDC &dc, int index )
{
    int h = m_lineHeight * index;
    wxRect rectToDraw(0, h, m_width, m_lineHeight);

    const SectionContext &sec = m_data[index];

    if (index == m_currSelIndex) {
        dc.SetBrush(m_currSelBrush);
        dc.DrawRectangle(rectToDraw);
    }

    int w = 0;
    dc.DrawText(wxString::Format("%08x-%08x", sec.Base, sec.Base + sec.Size - 1), w, h);
    w += m_widthRange;
    dc.DrawText(wxString::Format("%08x", sec.Size), w, h);
    w += m_widthSize;
    dc.DrawText(wxString::Format("%08x", sec.ModuleImageBase), w, h);
    w += m_widthModule;
    dc.DrawText(sec.Desc, w, h);
    w += m_widthDesc;
}

void MemInfoPanel::OnLeftDoubleClick( wxMouseEvent &event )
{
    //OnLeftDown(event);
    wxMessageBox(wxString::Format("Selection = %d", m_currSelIndex));
}

