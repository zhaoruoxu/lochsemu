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
    wxVSCROLL), m_insts(NULL)
    //m_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Lucida Console")
{
    InitLogic();
    InitRender();

    Bind(wxEVT_PAINT, &CpuPanel::OnPaint, this, wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &CpuPanel::OnEraseBackground, this, wxID_ANY);
    Bind(wxEVT_LEFT_DOWN, &CpuPanel::OnLeftDown, this, wxID_ANY);
    Bind(wxEVT_LEFT_UP, &CpuPanel::OnLeftUp, this, wxID_ANY);
    Bind(wxEVT_MOTION, &CpuPanel::OnMouseMove, this, wxID_ANY);
    Bind(wxEVT_LEAVE_WINDOW, &CpuPanel::OnMouseLeave, this, wxID_ANY);
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
    m_widthIp       = g_config.GetInt("CpuPanel", "WidthIp", 80);
    m_widthDisasm   = g_config.GetInt("CpuPanel", "WidthDisasm", 300);
    m_widthInfo     = g_config.GetInt("CpuPanel", "WidthInfo", 300);
    m_width         = m_widthIp + m_widthDisasm + m_widthInfo;

    m_minDistanceToBottom = g_config.GetInt("CpuPanel", "MinDistanceToBottom", 7);

    m_font = wxFont(g_config.GetInt("CpuPanel", "FontSize", 8), wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("CpuPanel", "FontName", "Lucida Console"));
    m_curlineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "CurrentLineColor", "#B100D7")));
    m_sellineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "SelectedLineColor", "#B0B0B0")));
    m_zerolineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "ZeroLineColor", "#D0D0D0")));
    m_jumpColor = wxColour(g_config.GetString("CpuPanel", "JumpColor", "#008080"));
    m_callRetColor = wxColour(g_config.GetString("CpuPanel", "CallRetColor", "#f00000"));

    m_showTargetNameInstead = g_config.GetInt("CpuPanel", "ShowTargetNameInsteadOfAddress", 1) != 0;
    m_jumpLineWidthMax  = g_config.GetInt("CpuPanel", "JumpLineWidthMax", 15);
    m_jumpLineWidthMin  = g_config.GetInt("CpuPanel", "JumpLineWidthMin", 3);
    m_jumpLineInstDistMax   = g_config.GetInt("CpuPanel", "JumpLineInstDistanceMax", 20);

    SetFont(m_font);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_currIndex = 0;
    m_currSelIndex = -1;
    m_isLeftDown = false;

    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_fontMetrics = dc.GetFontMetrics();
    m_lineHeight = m_fontMetrics.height + 2;

    SetScrollRate(10, m_lineHeight);
}

void CpuPanel::OnPaint( wxPaintEvent& event )
{
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
    const int idxStart = p.y / m_lineHeight - 1;
    const int idxEnd = (p.y + cs.GetHeight()) / m_lineHeight + 1;

    /* instructions */
    int index = 0;
    for (auto &inst : *m_insts) {
        if (idxStart <= index && index <= idxEnd)
            DrawInst(dc, inst.second, index);
        index++;
    }

    /* instruction jump lines */
    index = 0;
    for (auto &inst : *m_insts) {
        if (inst.second.target != 0 && m_insts->find(inst.second.target) != m_insts->end()) {
            int rindex = m_eipIndex[inst.second.target];
            if ((idxStart <= index && index <= idxEnd) || 
                (idxStart <= rindex && rindex <= idxEnd)) {
                int w0 = m_widthIp - 2;
                int w = w0 - CalcJumpLineWidth(index, rindex);
                int offset = m_lineHeight / 2;
                int h1 = index * m_lineHeight + offset;
                int h2 = rindex * m_lineHeight + offset;
                dc.SetPen(*wxBLACK_PEN);
                dc.DrawCircle(w0, h1, 1);
                dc.DrawLine(w0, h1, w, h1);
                dc.DrawLine(w, h1, w, h2);
                dc.DrawLine(w0, h2, w, h2);
            }
        }
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

    u32 opcode = inst.ptr->Main.Inst.Opcode;
    if (index == m_currIndex) {
        // highlight current line
        dc.SetBrush(m_curlineBrush);
        dc.DrawRectangle(rectToDraw);
    } else if (index == m_currSelIndex) {
        // highlight current selected line
        dc.SetBrush(m_sellineBrush);
        dc.DrawRectangle(rectToDraw);
    } else if (opcode == 0) {
        // darken zero line
        dc.SetBrush(m_zerolineBrush);
        dc.DrawRectangle(rectToDraw);
    }

    int h = m_lineHeight * index + 1;
    dc.SetBrush(*wxWHITE_BRUSH);

    // draw ip address
    dc.DrawText(wxString::Format("%08X", inst.eip), 0,h );

    wxString instr(inst.ptr->Main.CompleteInstr);
    if (opcode != 0) {
        if (instr.StartsWith("call") || instr.StartsWith("ret")) {
            dc.SetTextForeground(m_callRetColor);
        } else if (instr.StartsWith("j")) {
            dc.SetTextForeground(m_jumpColor);
        } else {
            dc.SetTextForeground(*wxBLACK);
        }
    }
    if (!inst.dllName.empty() || !inst.funcName.empty()) {
        wxString dll(inst.dllName);
        dc.DrawText(wxString::Format("%s<%s:%s>", inst.ptr->Main.Inst.Mnemonic, dll.SubString(0, dll.Length()-5), 
            inst.funcName.c_str()),
            m_widthIp, h);
    } else {
        dc.DrawText(instr, m_widthIp, h);
    }
    
    dc.SetTextForeground(*wxBLACK);
}

void CpuPanel::OnDataUpdate( const Disassembler::InstDisasmMap *insts )
{
    m_insts = insts;

    m_eipIndex.clear();
    int index = 0;
    for (auto &inst : *m_insts) {
        m_eipIndex[inst.first] = index++;
    }

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

void CpuPanel::OnLeftDown( wxMouseEvent& event )
{
    wxPoint p = event.GetPosition();
    wxPoint ps = GetViewStart();
    m_currSelIndex = ps.y + p.y / m_lineHeight;
    m_isLeftDown = true;

    Refresh();
}

void CpuPanel::OnLeftUp( wxMouseEvent& event )
{
    m_isLeftDown = false;
}

void CpuPanel::OnMouseMove( wxMouseEvent& event )
{
    if (!m_isLeftDown) return;
    wxPoint p = event.GetPosition();
    wxPoint ps = GetViewStart();
    m_currSelIndex = ps.y + p.y / m_lineHeight;
    Refresh();
}

void CpuPanel::OnMouseLeave( wxMouseEvent& event )
{
    m_isLeftDown = false;
}

int CpuPanel::CalcJumpLineWidth(int idx1, int idx2) const
{
    int dist = min(abs(idx1 - idx2), m_jumpLineInstDistMax);
    return m_jumpLineWidthMin + (m_jumpLineWidthMax - m_jumpLineWidthMin) * 
        dist / m_jumpLineInstDistMax;
}
