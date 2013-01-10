#include "stdafx.h"
#include "cpupanel.h"
#include "instruction.h"
#include "processor.h"

template <typename T>
static INLINE bool InRangeIncl(T val, T t0, T t1) {
    return t0 <= val && val <= t1;
}

CpuPanel::CpuPanel( wxWindow *parent ) : CustomScrolledControl(parent, wxSize(400, 400))
{
    m_mutex = MutexCS::Create();
    InitLogic();
    InitRender();

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

    wxFont f = wxFont(g_config.GetInt("CpuPanel", "FontSize", 8), wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("CpuPanel", "FontName", "Lucida Console"));
    UpdateFont(f);
    m_curlineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "CurrentLineColor", "#00fb3d")));
    m_sellineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "SelectedLineColor", "#B0B0B0")));
    m_zerolineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "ZeroLineColor", "#D0D0D0")));
    m_jumpColor = wxColour(g_config.GetString("CpuPanel", "JumpColor", "#0080ff"));
    m_callRetColor = wxColour(g_config.GetString("CpuPanel", "CallRetColor", "#f000f0"));

    m_showTargetNameInstead = g_config.GetInt("CpuPanel", "ShowTargetNameInsteadOfAddress", 1) != 0;
    m_jumpLineWidthMax  = g_config.GetInt("CpuPanel", "JumpLineWidthMax", 15);
    m_jumpLineWidthMin  = g_config.GetInt("CpuPanel", "JumpLineWidthMin", 3);
    m_jumpLineInstDistMax   = g_config.GetInt("CpuPanel", "JumpLineInstDistanceMax", 20);

    m_currIndex = -1;
    m_currSelIndex = -1;
    m_isLeftDown = false;
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
    if (inst.target == -1) return;
    int halfLine = m_lineHeight / 2;
    int h = inst.target > inst.eip ? 1 : -1;
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
    int halfLine = m_lineHeight / 2;
    for (auto &proc : m_procEntryEnd) {
        if (proc.first <= iend && proc.second >= istart) {
            int x0 = m_widthIp - 7, x1 = x0 - 5;
            int h0 = proc.second * m_lineHeight + halfLine, 
                h1 = proc.first * m_lineHeight + halfLine;
            dc.SetPen(wxPen(*wxBLACK, 3, wxPENSTYLE_SOLID));
            dc.DrawLine(x0, h0, x1, h0);
            dc.DrawLine(x1, h0, x1, h1);
            dc.DrawLine(x0, h1, x1, h1);
        }
    }

    // draw current selected line jump
    if (m_currSelIndex != -1 && m_insts[m_currSelEip].target != -1) {
        int rindex = -1; //m_eipIndex[m_insts[m_currSelEip].target];
        auto iter = m_eipIndex.find(m_insts[m_currSelEip].target);
        if (iter != m_eipIndex.end()) {
            rindex = iter->second;
        }
        if (rindex != -1 && IntersectAbs(rindex, m_currSelIndex, istart, iend)) {
            int x0 = m_widthIp - 3, x1 = x0 - 4;
            int h0 = m_currSelIndex * m_lineHeight + halfLine,
                h1 = rindex * m_lineHeight + halfLine;
            dc.SetPen(wxPen(*wxRED, 1, wxPENSTYLE_SOLID));
            //dc.DrawLine(x0, h0, x1, h0);
            dc.DrawLine(x1, h0, x1, h1);
            dc.DrawLine(x0, h1, x1, h1);
        }
    }

    if (m_currIndex != -1 && m_insts[m_currEip].target != -1) {
        int rindex = -1;
        auto iter = m_eipIndex.find(m_insts[m_currEip].target);
        if (iter != m_eipIndex.end()) {
            rindex = iter->second;
        }
        if (rindex != -1 && IntersectAbs(rindex, m_currIndex, istart, iend)) {
            int x0 = m_widthIp - 3, x1 = x0 - 4;
            int h0 = m_currIndex * m_lineHeight + halfLine,
                h1 = rindex * m_lineHeight + halfLine;
            dc.SetPen(wxPen(wxColour("#ff0080"), 2, wxPENSTYLE_SOLID));
            //dc.DrawLine(x0, h0, x1, h0);
            dc.DrawLine(x1, h0, x1, h1);
            dc.DrawLine(x0, h1, x1, h1);
        }
    }
}

void CpuPanel::OnDataUpdate( const Disassembler::InstDisasmMap *insts )
{
    {
        MutexCSLock lock(*m_mutex);
        m_insts = *insts;

        m_eipIndex.clear();
        m_indexEip.clear();
        int index = 0;
        for (auto &inst : m_insts) {
            m_eipIndex[inst.first] = index++;
            m_indexEip.push_back(inst.first);
        }

        m_procEntryEnd.clear();
        index = -1;
        int prevIndex = -1, prevRindex = -1;
        for (auto &inst : m_insts) {
            ++index;
            u32 entry   = inst.second.entry;
            if (entry == -1) continue;
            if (entry >= inst.second.eip) continue;
            auto riter  = m_eipIndex.find(entry);
            Assert(riter != m_eipIndex.end());
            int rindex  = riter->second;
            m_procEntryEnd[rindex] = index;
            //             if (prevRindex != -1 && prevIndex >= rindex) {
            //                 m_procEntryEnd[prevRindex] = rindex - 1;
            //             }
            prevIndex   = index;
            prevRindex  = rindex;
        }
    }

    m_currSelIndex  = -1;
    m_currSelEip    = 0;
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
    m_currEip   = m_indexEip[m_currIndex];
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
    m_currSelEip    = m_indexEip[m_currSelIndex];
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
    wxPoint p       = event.GetPosition();
    wxPoint ps      = GetViewStart();
    int selIndex    = ps.y + p.y / m_lineHeight;
    if (selIndex != m_currSelIndex) {
        m_currSelIndex  = selIndex;
        m_currSelEip    = m_indexEip[m_currSelIndex];
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
