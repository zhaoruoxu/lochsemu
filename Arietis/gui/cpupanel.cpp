#include "stdafx.h"
#include "cpupanel.h"
#include "instruction.h"
#include "processor.h"
#include "engine.h"

template <typename T>
static INLINE bool InRangeIncl(T val, T t0, T t1) {
    return t0 <= val && val <= t1;
}

CpuPanel::CpuPanel( wxWindow *parent, AEngine *engine ) : 
    SelectableScrolledControl(parent, wxSize(400, 200)), m_engine(engine)
{
    //m_mutex = MutexCS::Create();
    InitMenu();
    InitRender();

//     Bind(wxEVT_LEFT_DOWN, &CpuPanel::OnLeftDown, this, wxID_ANY);
//     Bind(wxEVT_LEFT_UP, &CpuPanel::OnLeftUp, this, wxID_ANY);
//     Bind(wxEVT_MOTION, &CpuPanel::OnMouseMove, this, wxID_ANY);
//     Bind(wxEVT_LEAVE_WINDOW, &CpuPanel::OnMouseLeave, this, wxID_ANY);
    Bind(wxEVT_RIGHT_DOWN, &CpuPanel::OnRightDown, this, wxID_ANY);

    m_insts = NULL;
}

CpuPanel::~CpuPanel()
{
    //MutexCS::Destroy(m_mutex);
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
    m_curlineBrush  = wxBrush(wxColor(g_config.GetString("CpuPanel", "CurrentLineColor", "#00fb3d")));
    m_sellineBrush  = wxBrush(wxColor(g_config.GetString("CpuPanel", "SelectedLineColor", "#B0B0B0")));
    m_zerolineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "ZeroLineColor", "#D0D0D0")));
    m_jumpColor     = wxColour(g_config.GetString("CpuPanel", "JumpColor", "#0080ff"));
    m_callRetColor  = wxColour(g_config.GetString("CpuPanel", "CallRetColor", "#f000f0"));

    m_showTargetNameInstead = g_config.GetInt("CpuPanel", "ShowTargetNameInsteadOfAddress", 1) != 0;
    m_jumpLineWidthMax      = g_config.GetInt("CpuPanel", "JumpLineWidthMax", 15);
    m_jumpLineWidthMin      = g_config.GetInt("CpuPanel", "JumpLineWidthMin", 3);
    m_jumpLineInstDistMax   = g_config.GetInt("CpuPanel", "JumpLineInstDistanceMax", 20);

    m_currIndex = -1;
    //m_currSelIndex = -1;
    //m_isLeftDown = false;
}

void CpuPanel::InitMenu()
{
    m_popup = new wxMenu;
    m_popup->Append(ID_PopupShowCurrInst, "&Show current instruction");
    m_popup->AppendSeparator();
    wxMenu *taintMenu = new wxMenu;
    taintMenu->Append(ID_PopupTaintReg, "Taint &register...");
    m_popup->AppendSubMenu(taintMenu, "Taint");

    Bind(wxEVT_COMMAND_MENU_SELECTED, &CpuPanel::OnPopupShowCurrInst, this, ID_PopupShowCurrInst);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &CpuPanel::OnPopupTaintReg, this, ID_PopupTaintReg); 
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
    if (m_insts == NULL) return;
    MutexCSLock lock(m_mutex);

    wxPoint pv  = GetViewStart();
    wxPoint p   = GetCurrentScrolledPos();
    wxSize cs   = GetClientSize();
    const int idxStart = pv.y;
    const int idxEnd = idxStart + cs.GetHeight() / m_lineHeight;

    /* instructions */
    int index = 0;
    for (InstPtr *inst = m_insts->Begin(); inst != m_insts->End(); inst = m_insts->Next(inst)) {
        if (idxStart <= index && index <= idxEnd)
            DrawInst(dc, *inst, index);
        index++;
    }

    /* jump lines */
    DrawJumpLines(dc, idxStart, idxEnd);

    /* vertical lines */
    dc.SetPen(*wxGREY_PEN);
    int lineX = m_widthIp + VertLineOffset;
    int lineY0 = p.y, lineY1 = p.y + cs.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDisasm;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthInfo;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
}

void CpuPanel::DrawInst( wxBufferedPaintDC &dc, const InstPtr inst, int index )
{
    wxRect rectToDraw(0, m_lineHeight * index, m_width, m_lineHeight);

    dc.SetPen(*wxTRANSPARENT_PEN);

    u32 opcode = inst->Main.Inst.Opcode;
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
    dc.DrawText(wxString::Format("%08X", inst->Eip), 0,h );
    
    DrawJumpIcon(dc, inst, index);

    wxString instr(inst->Main.CompleteInstr);
    if (opcode != 0) {
        if (instr.StartsWith("call") || instr.StartsWith("ret")) {
            dc.SetTextForeground(m_callRetColor);
        } else if (instr.StartsWith("j")) {
            dc.SetTextForeground(m_jumpColor);
        } else {
            dc.SetTextForeground(*wxBLACK);
        }
    }
    if (strlen(inst->DllName) > 0 || strlen(inst->FuncName) > 0) {
        wxString dll(inst->DllName);
        dc.DrawText(wxString::Format("%s<%s:%s>", inst->Main.Inst.Mnemonic, 
            dll.SubString(0, dll.Length()-5), inst->FuncName), m_widthIp, h);
    } else {
        dc.DrawText(instr, m_widthIp, h);
    }
    
    dc.SetTextForeground(*wxBLACK);
}


void CpuPanel::DrawJumpIcon( wxBufferedPaintDC &dc, const InstPtr inst, int index )
{
    if (inst->Target == -1) return;
    int halfLine = m_lineHeight / 2;
    int h = inst->Target > inst->Eip ? 1 : -1;
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
    if (m_currSelIndex != -1 && m_insts->GetInst(m_currSelEip)->Target != -1) {
        int rindex = -1; 
        u32 target = m_insts->GetInst(m_currSelEip)->Target;
        if (m_insts->IsInRange(target)) {
            rindex = m_insts->GetInst(target)->Index;
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

    if (m_currIndex != -1 && m_insts->GetInst(m_currEip)->Target != -1) {
        int rindex = -1;
        u32 target = m_insts->GetInst(m_currEip)->Target;
        if (m_insts->IsInRange(target)) {
            rindex = m_insts->GetInst(target)->Index;
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

void CpuPanel::OnDataUpdate( const InstSection *insts )
{
    {
        MutexCSLock lock(m_mutex);
        m_insts = insts;
        m_insts->Lock();

        m_procEntryEnd.clear();
        int prevIndex = -1, prevRindex = -1;
        for (InstPtr *inst = m_insts->Begin(); inst != m_insts->End(); inst = m_insts->Next(inst)) {
            int index = (*inst)->Index;
            u32 entry   = (*inst)->Entry;
            if (entry == -1) continue;
            if (entry >= (*inst)->Eip) continue;
            Assert(m_insts->GetInst(entry));
            int rindex  = m_insts->GetInst(entry)->Index;
            rindex = max(prevIndex+1, rindex);
            m_procEntryEnd[rindex] = index;
            prevIndex   = index;
            prevRindex  = rindex;
        }

        m_insts->Unlock();
    }

    m_currSelIndex  = -1;
    m_currSelEip    = 0;
    m_height = m_lineHeight * insts->GetCount();
    SetVirtualSize(m_width, m_height);
    //Refresh();
}

void CpuPanel::OnPtrChange( u32 addr )
{
    Assert(m_insts->GetInst(addr));
    m_currIndex = m_insts->GetInst(addr)->Index;
    m_currEip   = addr;

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

void CpuPanel::OnSelectionChange()
{
    m_currSelEip    = m_insts->GetEipFromIndex(m_currSelIndex);
}

int CpuPanel::CalcJumpLineWidth(int idx1, int idx2) const
{
    int dist = min(abs(idx1 - idx2), m_jumpLineInstDistMax);
    return m_jumpLineWidthMin + (m_jumpLineWidthMax - m_jumpLineWidthMin) * 
        dist / m_jumpLineInstDistMax;
}

void CpuPanel::OnRightDown( wxMouseEvent& event )
{
    PopupMenu(m_popup);
}

void CpuPanel::OnPopupShowCurrInst( wxCommandEvent &event )
{
    Scroll(0, m_currIndex);
}

void CpuPanel::OnPopupTaintReg( wxCommandEvent &event )
{
    static const wxString Regs[] = { 
        "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI", "EIP" 
    };

    int reg = wxGetSingleChoiceIndex("Select a register", "Taint", _countof(Regs), Regs);
    if (reg == -1) return;  // Cancelled

    wxString val = wxGetTextFromUser("Set Taint value (binary) for " + Regs[reg], "Taint");
    if (val.IsEmpty()) return; // Cancelled
    
    TaintEngine *te = m_engine->GetTaintEngine();
    te->Lock();
    if (reg < 8) {
        te->CpuTaint.GPRegs[reg] = Taint::FromBinString(val.ToStdString());
    } else if (reg == 8) {
        te->CpuTaint.Eip = Taint::FromBinString(val.ToStdString());
    } else {
        Assert(0);
    }
    te->Unlock();
}
