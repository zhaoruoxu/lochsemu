#include "stdafx.h"
#include "cpupanel.h"
#include "instruction.h"
#include "processor.h"

static bool InstCmp(const Disassembler::Inst &lhs, const Disassembler::Inst &rhs)
{
    return lhs.eip < rhs.eip;
}

CpuPanel::CpuPanel( wxWindow *parent )
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 400),
    wxVSCROLL | wxHSCROLL), m_insts(NULL)
    //m_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Lucida Console")
{
    InitLogic();
    InitRender();

    Bind(wxEVT_PAINT, &CpuPanel::OnPaint, this, wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &CpuPanel::OnEraseBackground, this, wxID_ANY);
    Bind(wxEVT_LEFT_DOWN, &CpuPanel::OnLeftClick, this, wxID_ANY);
}

int wxCALLBACK InstListItemCmp(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    if (item1 > item2)
        return 1;
    if (item1 < item2)
        return -1;
    return 0;
}

void CpuPanel::InitLogic()
{
    //std::make_heap(m_instVector.begin(), m_instVector.end(), InstCmp);
}

void CpuPanel::InitRender()
{
    m_widthIp       = g_config.GetInt("CpuPanel", "WidthIp", 70);
    m_widthDisasm   = g_config.GetInt("CpuPanel", "WidthDisasm", 300);
    m_widthInfo     = g_config.GetInt("CpuPanel", "WidthInfo", 300);
    m_width         = m_widthIp + m_widthDisasm + m_widthInfo;

    m_minDistanceToBottom = g_config.GetInt("CpuPanel", "MinDistanceToBottom", 7);

    m_font = wxFont(g_config.GetInt("CpuPanel", "FontSize", 8), wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("CpuPanel", "FontName", "Lucida Console"));
    m_curlineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "CurrentLineColor", "#00FF00")));
    m_sellineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "SelectedLineColor", "#B0B0B0")));

    SetFont(m_font);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

//     wxClientDC dc(this);
//     dc.SetFont(m_font);
//     m_fontMetrics = dc.GetFontMetrics();
    m_currIndex = 0;
    m_currSelIndex = -1;
    //SetScrollRate(1, m_fontMetrics.height);
}

void CpuPanel::OnPaint( wxPaintEvent& event )
{
//     wxPaintDC dc(this);
//     dc.SetBrush(wxBrush(wxColor("red"), wxBRUSHSTYLE_SOLID));
//     dc.DrawRectangle(wxPoint(0, 0), m_size);
    wxBufferedPaintDC dc(this);
    DoPrepareDC(dc);

    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    if (m_insts == NULL) return;
    Draw(dc);
}

void CpuPanel::OnEraseBackground( wxEraseEvent& event )
{

}

void CpuPanel::OnPreExecute( const Processor *cpu, const Instruction *inst )
{
}


wxPoint CpuPanel::GetCurrentScrolledPos() const
{
    int px, py;
    wxPoint vs = GetViewStart();
    GetScrollPixelsPerUnit(&px, &py);
    return wxPoint(vs.x * px, vs.y * py);
}


void CpuPanel::Draw( wxBufferedPaintDC &dc )
{
    static const int VertLineOffset = -2;
    wxPoint p = GetCurrentScrolledPos();
    wxSize cs = GetClientSize();
    int idxStart = p.y / m_lineHeight - 1;
    int idxEnd = (p.y + cs.GetHeight()) / m_lineHeight + 1;

    /* instructions */
    int index = 0;
    for (auto &inst : *m_insts) {
        if (idxStart <= index && index <= idxEnd)
            DrawInst(dc, inst.second, index);
        index++;
    }

    /* clear unwanted */
//     dc.SetPen(*wxWHITE_PEN);
//     dc.SetBrush(*wxWHITE_BRUSH);
//     dc.DrawRectangle(p.x + m_widthIp + m_widthDisasm + VertLineOffset, p.y, 
//         m_width, p.y + cs.GetHeight());

    dc.SetPen(*wxGREY_PEN);

    /* vertical lines */
    int lineX = p.x + m_widthIp + VertLineOffset;
    int lineY0 = p.y, lineY1 = p.y + cs.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDisasm;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthInfo;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
}

void CpuPanel::DrawInst( wxBufferedPaintDC &dc, const Disassembler::Inst &inst, int index )
{
    wxRect rectToDraw(0, m_lineHeight * index, m_width, m_lineHeight);
    wxRect rectScrolled = rectToDraw;
    CalcScrolledPosition(rectToDraw.x, rectToDraw.y, &rectScrolled.x, &rectScrolled.y);
    if (!IsExposed(rectScrolled)) return;

    dc.SetPen(*wxTRANSPARENT_PEN);
    // highlight current selected line
    if (index == m_currSelIndex) {
        dc.SetBrush(m_sellineBrush);
        dc.DrawRectangle(rectToDraw);
    }

    // highlight current line
    if (index == m_currIndex) {
        dc.SetBrush(m_curlineBrush);
        dc.DrawRectangle(rectToDraw);
    }

    int h = m_lineHeight * index + 1;
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawText(wxString::Format("%08X", inst.eip), 0,h );
    dc.DrawText(wxString::Format("%s", inst.instPtr->Main.CompleteInstr),
        m_widthIp, h);
    if (!inst.dllName.empty() || !inst.funcName.empty()) {
        dc.DrawText(wxString::Format("%s::%s", inst.dllName.c_str(), inst.funcName.c_str()),
            m_widthIp + m_widthDisasm, h);
    }
}

void CpuPanel::OnDataUpdate( const Disassembler::InstDisasmMap *insts )
{
    m_insts = insts;

    m_eipIndex.clear();
    int index = 0;
    for (auto &inst : *m_insts) {
        m_eipIndex[inst.first] = index++;
    }

    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_fontMetrics = dc.GetFontMetrics();
    m_lineHeight = m_fontMetrics.height + 2;

    SetScrollRate(10, m_lineHeight);

    m_height = m_lineHeight * insts->size();
    SetVirtualSize(m_width, m_height);

    Refresh();
}

void CpuPanel::OnPtrChange( u32 addr )
{
    auto iter = m_eipIndex.find(addr);
    if (iter == m_eipIndex.end()) {
        LxFatal("Cannot find index in GUI CpuPanel::EipIndex\n");
    }
    m_currIndex = iter->second;
    //Scroll(0, m_currIndex);

    wxPoint p = GetViewStart();
    wxSize cs = GetClientSize();
    int h = cs.GetHeight() / m_lineHeight;
    if (m_currIndex < p.y) {
        Scroll(0, m_currIndex);
    } else if (m_currIndex - p.y > h - m_minDistanceToBottom) {
        Scroll(0, m_currIndex - (h - m_minDistanceToBottom));
    }

    Refresh();
}

void CpuPanel::OnLeftClick( wxMouseEvent& event )
{
    wxClientDC dc(this);
    wxPoint p = event.GetLogicalPosition(dc);
    wxPoint ps = GetViewStart();
    m_currSelIndex = ps.y + p.y / m_lineHeight;

    Refresh();
}
