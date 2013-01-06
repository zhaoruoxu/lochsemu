#include "stdafx.h"
#include "cpupanel.h"
#include "instruction.h"
#include "processor.h"

template <typename T>
static INLINE bool InRangeIncl(T val, T t0, T t1) {
    return t0 <= val && val <= t1;
}

CpuPanel::CpuPanel( wxWindow *parent )
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 400), wxVSCROLL)
    //m_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Lucida Console")
{
    m_mutex = MutexCS::Create();
    InitLogic();
    InitRender();

    Bind(wxEVT_PAINT, &CpuPanel::OnPaint, this, wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &CpuPanel::OnEraseBackground, this, wxID_ANY);
    Bind(wxEVT_LEFT_DOWN, &CpuPanel::OnLeftDown, this, wxID_ANY);
    Bind(wxEVT_LEFT_UP, &CpuPanel::OnLeftUp, this, wxID_ANY);
    Bind(wxEVT_MOTION, &CpuPanel::OnMouseMove, this, wxID_ANY);
    Bind(wxEVT_LEAVE_WINDOW, &CpuPanel::OnMouseLeave, this, wxID_ANY);
    Bind(wxEVT_RIGHT_DOWN, &CpuPanel::OnRightDown, this, wxID_ANY);
}

CpuPanel::~CpuPanel()
{
    MutexCS::Destroy(m_mutex);
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
    m_curlineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "CurrentLineColor", "#00fb3d")));
    m_sellineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "SelectedLineColor", "#B0B0B0")));
    m_zerolineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "ZeroLineColor", "#D0D0D0")));
    m_jumpColor = wxColour(g_config.GetString("CpuPanel", "JumpColor", "#0080ff"));
    m_callRetColor = wxColour(g_config.GetString("CpuPanel", "CallRetColor", "#f000f0"));

    m_showTargetNameInstead = g_config.GetInt("CpuPanel", "ShowTargetNameInsteadOfAddress", 1) != 0;
    m_jumpLineWidthMax  = g_config.GetInt("CpuPanel", "JumpLineWidthMax", 15);
    m_jumpLineWidthMin  = g_config.GetInt("CpuPanel", "JumpLineWidthMin", 3);
    m_jumpLineInstDistMax   = g_config.GetInt("CpuPanel", "JumpLineInstDistanceMax", 20);

    SetFont(m_font);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_currIndex = -1;
    m_currSelIndex = -1;
    m_isLeftDown = false;

    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_fontMetrics = dc.GetFontMetrics();
    m_lineHeight = m_fontMetrics.height + 2;

    SetScrollRate(1, m_lineHeight);
}

void CpuPanel::OnPaint( wxPaintEvent& event )
{
    wxBufferedPaintDC dc(this);
    DoPrepareDC(dc);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    //if (m_insts == NULL) return;
    
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
    MutexCSLock lock(*m_mutex);

    static const int VertLineOffset = -2;
    wxPoint pv = GetViewStart();
    wxPoint p = GetCurrentScrolledPos();
    wxSize cs = GetClientSize();
    const int idxStart = pv.y;
    const int idxEnd = idxStart + cs.GetHeight() / m_lineHeight;//(p.y + cs.GetHeight()) / m_lineHeight + 1;

    /* instructions */
    int index = 0;
    for (auto &inst : m_insts) {
        if (idxStart <= index && index <= idxEnd)
            DrawInst(dc, inst.second, index);
        index++;
    }

    DrawJumpLines(dc, idxStart, idxEnd);

    /* clear unwanted */
//     dc.SetPen(*wxWHITE_PEN);
//     dc.SetBrush(*wxWHITE_BRUSH);
//     dc.DrawRectangle(p.x + m_widthIp + m_widthDisasm + VertLineOffset, p.y, 
//         m_width, p.y + cs.GetHeight());

    dc.SetPen(*wxGREY_PEN);

    /* vertical lines */
    int lineX = m_widthIp + VertLineOffset;
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
//     wxRect rectScrolled = rectToDraw;
//     CalcScrolledPosition(rectToDraw.x, rectToDraw.y, &rectScrolled.x, &rectScrolled.y);
//     if (!IsExposed(rectScrolled)) return;

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
    
    DrawJumpIcon(dc, inst, index);

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


void CpuPanel::DrawJumpIcon( wxBufferedPaintDC &dc, const Disassembler::Inst &inst, int index )
{
    int rindex = -1;
    if (inst.target != -1 && m_insts.find(inst.target) != m_insts.end()) {
        rindex = m_eipIndex[inst.target];
    } else {
        return;
    }
    int halfLine = m_lineHeight / 2;
    int h = rindex > index ? 1 : -1;
    wxPoint pLeft(m_widthIp - 7 - h * 3, index * m_lineHeight + halfLine);
    wxPoint pRight(m_widthIp - 7 + h * 4, index * m_lineHeight + halfLine - h); 
    
    wxPoint pMid(m_widthIp - 7, index * m_lineHeight + halfLine + h * 3);
    dc.SetPen(wxPen(*wxRED, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(pLeft, pMid);
    dc.DrawLine(pMid, pRight);
}


void CpuPanel::DrawJumpLines( wxBufferedPaintDC &dc, int istart, int iend )
{
    /* instruction jump lines */
    int index = 0;
    int halfLine = m_lineHeight / 2;
    for (auto &inst : m_insts) {
        if (inst.second.entry != -1) {
            int rindex = m_eipIndex[inst.second.entry];
            Assert(rindex >= 0);

            if (rindex < index) {
                int x0 = m_widthIp - 7, x1 = x0 - 5;
                int h0 = index * m_lineHeight + halfLine, 
                    h1 = rindex * m_lineHeight + halfLine;
                dc.SetPen(wxPen(*wxBLACK, 3, wxPENSTYLE_SOLID));
                dc.DrawLine(x0, h0, x1, h0);
                dc.DrawLine(x1, h0, x1, h1);
                dc.DrawLine(x0, h1, x1, h1);
            }
        }

        if (index == m_currIndex || index == m_currSelIndex) {
            int rindex = -1;
            if (inst.second.target != -1 && m_insts.find(inst.second.target) != m_insts.end()) {
                rindex = m_eipIndex[inst.second.target];
            }
            if (rindex != -1) {
                int x0 = m_widthIp - 3, x1 = x0 - 4;
                int h0 = index * m_lineHeight + halfLine,
                    h1 = rindex * m_lineHeight + halfLine;
                dc.SetPen(wxPen(*wxRED, 2, wxPENSTYLE_SOLID));
                //dc.DrawLine(x0, h0, x1, h0);
                dc.DrawLine(x1, h0, x1, h1);
                dc.DrawLine(x0, h1, x1, h1);
            }
        }
        index++;
    }
}

void CpuPanel::OnDataUpdate( const Disassembler::InstDisasmMap *insts )
{
    {
        MutexCSLock lock(*m_mutex);
        m_insts = *insts;

        m_eipIndex.clear();
        int index = 0;
        for (auto &inst : m_insts) {
            m_eipIndex[inst.first] = index++;
        }
    }

    m_height = m_lineHeight * insts->size();
    SetVirtualSize(m_width, m_height);
    //Refresh();
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
    Update();
}

void CpuPanel::OnLeftDown( wxMouseEvent& event )
{
    wxPoint p       = event.GetPosition();
    wxPoint ps      = GetViewStart();
    m_currSelIndex  = ps.y + p.y / m_lineHeight;
    m_isLeftDown    = true;

    Refresh();
}

void CpuPanel::OnLeftUp( wxMouseEvent& event )
{
    m_isLeftDown    = false;
}

void CpuPanel::OnMouseMove( wxMouseEvent& event )
{
    if (!m_isLeftDown) return;
    wxPoint p = event.GetPosition();
    wxPoint ps = GetViewStart();
    int selIndex = ps.y + p.y / m_lineHeight;
    if (selIndex != m_currSelIndex) {
        m_currSelIndex = selIndex;
        Refresh();
    }
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

void CpuPanel::OnRightDown( wxMouseEvent& event )
{
    Scroll(0, m_currIndex);
}
