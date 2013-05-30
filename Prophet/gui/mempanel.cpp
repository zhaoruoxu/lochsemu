#include "stdafx.h"
#include "mempanel.h"
#include "engine.h"
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
    Base        = sec.base;
    Size        = sec.size;
    Desc        = sec.Desc;
    ModuleImageBase = minfo->ImageBase;
    ModuleName  = minfo->Name;
}

SectionContext::SectionContext()
{
    Base        = 0;
    Size        = 0;
    Desc        = "n/a";
    ModuleImageBase = 0;
    ModuleName  = "n/a";
    
}


MemDataPanel::MemDataPanel( wxWindow *parent, ProphetFrame *dad, ProEngine *engine )
    : CustomScrolledControl(parent, wxSize(400, 150)), m_engine(engine), m_dad(dad)
{
    Bind(wxEVT_LEFT_DOWN,   &MemDataPanel::OnLeftDown,      this, wxID_ANY);
    Bind(wxEVT_LEFT_UP,     &MemDataPanel::OnLeftUp,        this, wxID_ANY);
    Bind(wxEVT_MOTION,      &MemDataPanel::OnMouseMove,     this, wxID_ANY);
    Bind(wxEVT_LEAVE_WINDOW,&MemDataPanel::OnMouseLeave,    this, wxID_ANY);
    Bind(wxEVT_RIGHT_DOWN,  &MemDataPanel::OnRightClick,    this, wxID_ANY);

    InitMenu();
    InitRender();
    m_section   = NULL;
    //m_taint     = m_engine->GetTaintEngine();

    m_isLeftDown = false;
    m_selDown = m_selUp = -1;
}


MemDataPanel::~MemDataPanel()
{
    //SAFE_DELETE(m_popup);
}

void MemDataPanel::InitRender()
{
    wxFont f = wxFont(g_config.GetInt("MemDataPanel", "FontSize", 9),
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false,
        g_config.GetString("MemInfoPanel", "FontName", "Lucida Console"));
    UpdateFont(f);

    m_bgBrush       = wxBrush(wxColour(g_config.GetString("MemDataPanel", "SelBgColor", "#C0C0C0")));
    m_widthOffset   = g_config.GetInt("MemDataPanel", "WidthOffset", 70);
    m_widthHex      = g_config.GetInt("MemDataPanel", "WidthHex", 18);
    m_widthChar     = g_config.GetInt("MemDataPanel", "WidthChar", 7);
    m_widthTaint    = g_config.GetInt("MemDataPanel", "WidthTaint", 64);
    m_width         = m_widthOffset + (m_widthHex + m_widthChar + m_widthTaint) * CharsPerLine;
}

void MemDataPanel::Draw( wxBufferedPaintDC &dc )
{
    if (NULL == m_section) {
        dc.SetBackground(*wxGREY_BRUSH);
        dc.Clear();
        return;
    }

    MutexCSLock lock(m_mutex);

    wxPoint viewStart   = GetViewStart();
    wxSize clientsize   = GetClientSize();
    const int istart    = viewStart.y;
    const int iend      = istart + clientsize.GetHeight() / m_lineHeight;

    dc.SetPen(*wxTRANSPARENT_PEN);
    for (int i = istart; i <= min(m_totalLines-1, iend); i++)
        DrawLine(dc, i);

    dc.SetPen(*wxGREY_PEN);
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);

    int lineX = m_widthOffset;
    int lineY0 = viewStart.y * py, lineY1 = lineY0 + clientsize.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthHex * CharsPerLine/2;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthHex * CharsPerLine/2;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthChar * CharsPerLine;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    for (int i = 0; i < CharsPerLine; i++) {
        lineX += m_widthTaint;
        dc.DrawLine(lineX, lineY0, lineX, lineY1);
    }
}

static bool InSelRange(int o, int sel1, int sel2)
{
    return o >= min(sel1, sel2) && o <= max(sel1, sel2);
}

void MemDataPanel::DrawLine( wxBufferedPaintDC &dc, int idx )
{
    int h = m_lineHeight * idx;
    u32 offset = CharsPerLine * idx;
    dc.DrawText(wxString::Format("%08x", m_section->Base() + offset), 0, h);
    int w = m_widthOffset;
    for (int i = 0; i < CharsPerLine; i++) {

        if (InSelRange((int) offset, m_selDown, m_selUp)) {
            dc.SetBrush(m_bgBrush);
            dc.DrawRectangle(w, h, m_widthHex, m_lineHeight);
        }
        dc.DrawText(wxString::Format("%02x", m_rawdata[offset]), w, h);
        offset++;
        w += m_widthHex;
    }
    offset = CharsPerLine * idx;
    for (int i = 0; i < CharsPerLine; i++) {
        if (InSelRange((int) offset, m_selDown, m_selUp)) {
            dc.SetBrush(m_bgBrush);
            dc.DrawRectangle(w, h, m_widthChar, m_lineHeight);
        }
        byte b = m_rawdata[offset];
        char c = (b >= 0x20 && b <= 0x7e) ? (char) b : '.';
        dc.DrawText(wxString::Format("%c", c), w, h);
        offset++;
        w += m_widthChar;
    }
//     offset = m_context.Base + CharsPerLine * idx;
//     for (int i = 0; i < CharsPerLine; i++) {
//         bool highlight = InSelRange((int) offset - m_context.Base, m_selDown, m_selUp);
//         DrawTaint(dc, m_taint->MemTaint.Get<1>(offset), 
//             wxRect(w, h, m_widthTaint, m_lineHeight), highlight);
//         offset++;
//         w += m_widthTaint;
//     }
}

void MemDataPanel::UpdateData( const Section *sec, const SectionContext &ctx )
{
    Assert(sec);
    if (sec == m_section) return;

    {
        MutexCSLock lock(m_mutex);

        m_section   = sec;
        m_context   = ctx;
        m_rawdata   = sec->GetRawData(sec->Base());
        m_totalLines = sec->Size() / CharsPerLine;
        Scroll(0, 0);
    }
    SetVirtualSize(m_width, m_totalLines * m_lineHeight);
    Refresh();

}

void MemDataPanel::SelectAddress( u32 addr, u32 len )
{
    // todo 
    Assert(InRange(addr, m_context.Base, m_context.Size));
    m_selDown   = (int) addr - m_context.Base;
    m_selUp     = m_selDown + len - 1;
    Scroll(0, (addr - m_context.Base) / CharsPerLine);
    Refresh();
}

void MemDataPanel::OnLeftDown( wxMouseEvent &event )
{
    wxPoint p   = event.GetPosition();
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);
    wxPoint viewStart = GetViewStart();

    p.x += px * viewStart.x;
    p.y += py * viewStart.y;

    if (!InRange(p.x, m_widthOffset, m_widthHex * CharsPerLine)) {
        m_selDown = m_selUp = -1;
        Refresh();
        return;
    }

    m_selDown       = GetIndex(p);
    m_selUp         = m_selDown;
    m_isLeftDown    = true;
    Refresh();
    //wxMessageBox(wxString::Format("%x", m_selStart));
}

void MemDataPanel::OnLeftUp( wxMouseEvent &event )
{
    m_isLeftDown    = false;
}

void MemDataPanel::OnMouseMove( wxMouseEvent &event )
{
    static int prevSelUp = m_selUp;
    if (!m_isLeftDown) return;
    wxPoint p = event.GetPosition();
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);
    wxPoint vs = GetViewStart();

    p.x += px * vs.x;
    p.y += py * vs.y;

    m_selUp = GetIndex(p);
    if (m_selUp != prevSelUp)
        Refresh();
}

void MemDataPanel::OnMouseLeave( wxMouseEvent &event )
{
    //m_isLeftDown    = false;
}

int MemDataPanel::GetIndex( const wxPoint &mouse )
{
    wxPoint p = mouse;

    p.x -= m_widthOffset;
    if (p.x < 0)
        p.x = 0;
    if (p.x >= m_widthHex * CharsPerLine)
        p.x = m_widthHex * CharsPerLine - 1;
    
    return p.y / m_lineHeight * CharsPerLine + p.x / m_widthHex;
}

void MemDataPanel::InitMenu()
{
    //m_popup = new wxMenu;
    //wxMenu *taintMem = new wxMenu;
    //taintMem->Append(ID_PopupTaintMemRange, "Cyclic");
    //taintMem->Append(ID_PopupTaintMemRangeAllBits, "All at once");
    //m_popup->AppendSubMenu(taintMem, "Taint Memory Ranged");

//     Bind(wxEVT_COMMAND_MENU_SELECTED, &MemDataPanel::OnTaintMemoryRangedClicked, 
//         this, ID_PopupTaintMemRange);
//     Bind(wxEVT_COMMAND_MENU_SELECTED, &MemDataPanel::OnTaintMemoryRangedAllBitsClicked,
//         this, ID_PopupTaintMemRangeAllBits);
}

// void MemDataPanel::OnTaintMemoryRangedClicked( wxCommandEvent &event )
// {
//     TaintMemRanged(false);
// }
// 
// void MemDataPanel::OnTaintMemoryRangedAllBitsClicked( wxCommandEvent &event )
// {
//     TaintMemRanged(true);
// }

void MemDataPanel::OnRightClick( wxMouseEvent &event )
{
    //PopupMenu(m_popup);
}

// void MemDataPanel::TaintMemRanged( bool allbits )
// {
//     if (m_selDown == -1 || m_selUp == -1) {
//         wxMessageBox("No selection");
//         return;
//     }
// 
//     int start = min(m_selDown, m_selUp);
//     int len = abs(m_selDown - m_selUp) + 1;
//     m_taint->TaintMemoryRanged((uint) m_context.Base + start, (uint) len, allbits);
//     Refresh();
// }

MemInfoPanel::MemInfoPanel(wxWindow *parent, ProphetFrame *dad, MemDataPanel *data)
    : SelectableScrolledControl(parent, wxSize(400, 150)), m_dataPanel(data), m_dad(dad)
{
    InitRender();
    m_memory = NULL;

    Bind(wxEVT_LEFT_DCLICK,     &MemInfoPanel::OnLeftDoubleClick, this, wxID_ANY);
}

MemInfoPanel::~MemInfoPanel()
{
}

void MemInfoPanel::UpdateData( const Emulator *emu, const Memory *mem )
{
    {
        MutexCSLock lock(m_mutex);
        m_memory = mem;
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
    wxFont f = wxFont(g_config.GetInt("MemInfoPanel", "FontSize", 9),
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
    MutexCSLock lock(m_mutex);
    
    wxPoint viewStart   = GetViewStart();
    wxSize clientSize   = GetClientSize();
    const int istart    = viewStart.y;
    const int iend      = istart + clientSize.GetHeight() / m_lineHeight;

    for (int i = istart; i <= min((int) m_data.size()-1, iend); i++) {
        DrawItem(dc, i);
    }

    dc.SetPen(*wxGREY_PEN);
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
    if (m_currSelIndex >= (int) m_data.size() || m_currSelIndex < 0) return;
    const SectionContext &ctx = m_data[m_currSelIndex];
    const Section *sec = m_memory->GetSection(ctx.Base);

    if (!sec->IsAllCommitted()) {
        wxMessageBox("Cannot show data of this section:\n\nSection contains uncommitted pages",
            "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    m_dataPanel->UpdateData(sec, ctx);
}


