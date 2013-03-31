#include "stdafx.h"
#include "cpupanel.h"
#include "instruction.h"
#include "processor.h"
#include "engine.h"
#include "mainframe.h"

#include "dbg/breakpoint.h"

template <typename T>
static INLINE bool InRangeIncl(T val, T t0, T t1) {
    return t0 <= val && val <= t1;
}

CpuPanel::CpuPanel( wxWindow *parent, ProphetFrame *dad, ProEngine *engine ) : 
    SelectableScrolledControl(parent, wxSize(400, 200)), m_engine(engine), m_dad(dad)
{
    InitMenu();
    InitRender();

    Bind(wxEVT_RIGHT_DOWN,  &CpuPanel::OnRightDown,     this, wxID_ANY);
    Bind(wxEVT_MIDDLE_DOWN, &CpuPanel::OnMiddleDown,    this, wxID_ANY);

    m_insts         = NULL;
    //m_cpu           = NULL;
    m_currSelEip    = 0;
    m_currEip       = 0;
    m_isbusy        = false;
}

CpuPanel::~CpuPanel()
{
}

void CpuPanel::InitRender()
{
    m_widthIp       = g_config.GetInt("CpuPanel", "WidthIp", 80);
    m_widthDisasm   = g_config.GetInt("CpuPanel", "WidthDisasm", 300);
    m_widthInfo     = g_config.GetInt("CpuPanel", "WidthInfo", 300);
    m_width         = m_widthIp + m_widthDisasm + m_widthInfo;

    m_minDistanceToEdge = g_config.GetInt("CpuPanel", "MinDistanceToEdge", 10);

    wxFont f = wxFont(g_config.GetInt("CpuPanel", "FontSize", 9), wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("CpuPanel", "FontName", "Lucida Console"));
    UpdateFont(f);
    m_curlineBrush  = wxBrush(wxColor(g_config.GetString("CpuPanel", "CurrentLineColor", "#00fb3d")));
    m_sellineBrush  = wxBrush(wxColor(g_config.GetString("CpuPanel", "SelectedLineColor", "#B0B0B0")));
    m_zerolineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "ZeroLineColor", "#D0D0D0")));
    m_jumpColor     = wxColour(g_config.GetString("CpuPanel", "JumpColor", "#0080ff"));
    m_callRetColor  = wxColour(g_config.GetString("CpuPanel", "CallRetColor", "#f000f0"));

    m_showTargetNameInstead = g_config.GetInt("CpuPanel", "ShowTargetNameInsteadOfAddress", 1) != 0;
}

void CpuPanel::InitMenu()
{
    m_popup = new wxMenu;
    m_popup->Append(ID_PopupShowCurrInst, "Show &Current Instruction");
    m_popup->Append(ID_PopupShowTargetInst, "Show &Target Instruction");
    m_popup->AppendSeparator();
    wxMenu *taintMenu = new wxMenu;
    taintMenu->Append(ID_PopupTaintReg, "Taint &Register...");
    m_popup->AppendSubMenu(taintMenu, "Taint");
    m_popup->Append(ID_PopupTrackMemory, "Track Memory...");

    Bind(wxEVT_COMMAND_MENU_SELECTED, &CpuPanel::OnPopupShowCurrInst,   this, ID_PopupShowCurrInst);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &CpuPanel::OnPopupShowTargetInst, this, ID_PopupShowTargetInst);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &CpuPanel::OnPopupTaintReg,       this, ID_PopupTaintReg); 
    Bind(wxEVT_COMMAND_MENU_SELECTED, &CpuPanel::OnPopupTrackMemory,    this, ID_PopupTrackMemory);
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
    if (m_insts == NULL) {
        dc.SetBackground(*wxGREY_BRUSH);
        dc.Clear();
        return;
    }

    if (m_isbusy) return;

    SyncObjectLock lock(*m_insts);

    wxPoint pv  = GetViewStart();
    wxPoint p   = GetCurrentScrolledPos();
    wxSize cs   = GetClientSize();
    const int idxStart = pv.y;
    const int idxEnd = idxStart + cs.GetHeight() / m_lineHeight;

    int currIndex = m_insts->GetInst(m_currEip)->Index;

    /* instructions */
    int index = 0;
    for (InstPtr *inst = m_insts->Begin(); inst != m_insts->End(); inst = m_insts->Next(inst)) {
        Assert(index == (*inst)->Index);
        if (idxStart <= index && index <= idxEnd)
            DrawInst(dc, *inst, index, currIndex);
        index++;
    }
    if (m_insts->IsInRange(m_currSelEip))
        DrawJumpLine(dc, m_insts->GetInst(m_currSelEip), m_currSelIndex);
    else if (m_insts->IsInRange(m_currEip))
        DrawJumpLine(dc, m_insts->GetInst(m_currEip), currIndex);

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

void CpuPanel::DrawInst( wxBufferedPaintDC &dc, const InstPtr inst, int index, int currIndex )
{
    wxRect rectToDraw(0, m_lineHeight * index, m_width, m_lineHeight);

    dc.SetPen(*wxTRANSPARENT_PEN);

    u32 opcode = inst->Main.Inst.Opcode;
    if (index == currIndex) {
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

    // draw breakpoint
    const Breakpoint *bp = m_engine->GetDebugger()->GetBreakpoint(inst->Eip);
    if (bp != NULL) {
        wxRect rectBp(0, h, m_widthIp, m_lineHeight);
        if (bp->Enabled) {
            if (bp->Desc == "crt_entry") {
                dc.SetBrush(wxBrush(wxColour("#8080ff")));
            } else {
                dc.SetBrush(wxColour("#00c000"));
            }
        } else {
            dc.SetBrush(*wxRED_BRUSH);
        }
        dc.DrawRectangle(rectBp);
    }

    // draw ip address
    dc.DrawText(wxString::Format("%08X", inst->Eip), 0, h);
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
    if (strlen(inst->TargetModuleName) > 0 && strlen(inst->TargetFuncName) > 0) {
        wxString dll(inst->TargetModuleName);
        dc.DrawText(wxString::Format("%s<%s:%s>", instr, 
            dll.SubString(0, dll.Length()-5), inst->TargetFuncName), m_widthIp, h);
    } else {
        dc.DrawText(instr, m_widthIp, h);
    }

    if (inst->Desc.size() > 0) {
        dc.SetTextForeground(*wxBLACK);
        dc.DrawText(inst->Desc, m_widthIp + m_widthDisasm, h);
    }

    dc.SetTextForeground(*wxBLACK);
}

void CpuPanel::DrawJumpLine( wxBufferedPaintDC &dc, const InstPtr inst, int index )
{
    if (inst->Target == -1) return;
    if (!m_insts->IsInRange(inst->Target)) return;

    int rindex = m_insts->GetInst(inst->Target)->Index;
    int w = m_widthIp - 7;
    const int HalfLine = m_lineHeight / 2;
    int h0 = index * m_lineHeight + HalfLine;
    int h1 = rindex * m_lineHeight + HalfLine;
    dc.SetPen(wxPen(*wxBLUE, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(w, h0, w, h1);
    dc.DrawLine(w, h1, w+5, h1);
}


void CpuPanel::DrawJumpIcon( wxBufferedPaintDC &dc, const InstPtr inst, int index )
{
    if (inst->Target == -1) return;
    if (!m_insts->IsInRange(inst->Target)) return;
    int halfLine = m_lineHeight / 2;
    int h = inst->Target > inst->Eip ? 1 : -1;
    wxPoint pLeft(m_widthIp - 7 - h * 3, index * m_lineHeight + halfLine);
    wxPoint pRight(m_widthIp - 7 + h * 4, index * m_lineHeight + halfLine - h); 
    
    wxPoint pMid(m_widthIp - 7, index * m_lineHeight + halfLine + h * 3);
    dc.SetPen(wxPen(*wxBLUE, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(pLeft, pMid);
    dc.DrawLine(pMid, pRight);
}

void CpuPanel::OnDataUpdate( u32 addr )
{
    if (m_insts == NULL || !m_insts->IsInRange(addr)) {
        m_insts = m_disasm->GetInstSection(addr);
    }

    m_currSelIndex  = -1;
    m_currSelEip    = 0;
    m_height        = m_lineHeight * m_insts->GetCount();
    SetVirtualSize(m_width, m_height);
}

void CpuPanel::OnCurrentEipChange( u32 addr )
{
    m_currEip   = addr;
    OnDataUpdate(addr);
    int currIndex = m_insts->GetInst(m_currEip)->Index;
    ScrollProperly(currIndex);
    Refresh();
}

void CpuPanel::ShowCode( u32 addr )
{
    OnDataUpdate(addr);
    m_currSelIndex = m_insts->GetInst(addr)->Index;
    //OnSelectionChange();
    m_currSelEip = addr;
    ScrollProperly(m_currSelIndex);
    Refresh();
}

void CpuPanel::ScrollProperly( int index )
{
    wxPoint p = GetViewStart();
    wxSize cs = GetClientSize();
    int h = cs.GetHeight() / m_lineHeight;
    if (index < p.y) {
        Scroll(0, max(index - m_minDistanceToEdge, 0));
    } else if (index - p.y > h - m_minDistanceToEdge) {
        Scroll(0, index - (h - m_minDistanceToEdge));
    }
}

void CpuPanel::OnSelectionChange()
{
    m_currSelEip    = m_insts->GetEipFromIndex(m_currSelIndex);
}

void CpuPanel::OnRightDown( wxMouseEvent& event )
{
    OnLeftDown(event);
    OnLeftUp(event);
    PopupMenu(m_popup);
}

void CpuPanel::OnPopupShowCurrInst( wxCommandEvent &event )
{
    //Assert(m_cpu);
    OnCurrentEipChange(m_currEip);
}

void CpuPanel::OnPopupShowTargetInst( wxCommandEvent &event )
{
    u32 target = (u32) -1;
    if (m_currSelEip != 0) {
        target = m_disasm->GetInst(m_currSelEip)->Target;
    } else {
        target = m_disasm->GetInst(m_currEip)->Target;
    }
    if (target == (u32) -1) {
        wxMessageBox("No target");
        return;
    }
    ShowCode(target);
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
    
    NOT_IMPLEMENTED();

//     TaintEngine *te = m_engine->GetTaintEngine();
//     {
//         SyncObjectLock lock(*te);
//         if (reg < 8) {
//             te->CpuTaint.GPRegs[reg][0] = Taint::FromBinString(val.ToStdString());
//         } else if (reg == 8) {
//             te->CpuTaint.Eip[0] = Taint::FromBinString(val.ToStdString());
//         } else {
//             Assert(0);
//         }
//     }

}

void CpuPanel::OnPopupTrackMemory( wxCommandEvent &event )
{
    if (GetSelectedEip() != 0) {
        TrackMemory(GetSelectedEip());
    } else if (GetCurrentEip() != 0) {
        TrackMemory(GetCurrentEip());
    } else {
        wxMessageBox("No instruction selected!");
    }
}

void CpuPanel::TrackMemory( u32 instEip )
{
    InstPtr inst = m_insts->GetInst(instEip);
    wxArrayString strs;
    strs.Add(wxString::Format("ARG1: %s", IsMemoryArg(inst->Main.Argument1) ? 
        inst->Main.Argument1.ArgMnemonic : "N/A"));
    strs.Add(wxString::Format("ARG2: %s", IsMemoryArg(inst->Main.Argument2) ? 
        inst->Main.Argument2.ArgMnemonic : "N/A"));
    strs.Add(wxString::Format("ARG3: %s", IsMemoryArg(inst->Main.Argument3) ? 
        inst->Main.Argument3.ArgMnemonic : "N/A"));
    strs.Add(wxString::Format("ADDR: %08x", (u32) inst->Main.Inst.AddrValue));
    strs.Add(wxString::Format("IMM:  %08x", (u32) inst->Main.Inst.Immediat));
    strs.Add(wxString::Format("EIP:  %08x", instEip));
    int ch = wxGetSingleChoiceIndex("Select address", "Track memory", strs, 0);
    if (ch == -1) return;

    u32 memAddr = 0;
    const Processor *cpu = m_debugger->GetCurrentThread()->CPU();
    switch (ch) {
    case 0:
        {
            if (IsMemoryArg(inst->Main.Argument1))
                memAddr = cpu->Offset32(inst->Main.Argument1);
        } break;
    case 1:
        {
            if (IsMemoryArg(inst->Main.Argument2)) 
                memAddr = cpu->Offset32(inst->Main.Argument2);
        } break;
    case 2:
        {
            if (IsMemoryArg(inst->Main.Argument3))
                memAddr = cpu->Offset32(inst->Main.Argument3);
        } break;
    case 3:
        {
            memAddr = (u32) inst->Main.Inst.AddrValue;
        } break;
    case 4:
        {
            memAddr = (u32) inst->Main.Inst.Immediat;
        } break;
    case 5:
        {
            memAddr = instEip;
        } break;
    default:
        break;
    }

    if (memAddr == 0) {
        wxMessageBox("Invalid selection!");
        return;
    }

    m_dad->ShowInMemory(memAddr);
}

void CpuPanel::OnProcessPostLoad( ProcessPostLoadEvent &event )
{
    m_disasm = m_engine->GetDisassembler();
    m_debugger = m_engine->GetDebugger();
}

// void CpuPanel::OnPreExecute( PreExecuteEvent &event )
// {
//     m_cpu = event.Cpu;
//     m_currEip = event.Cpu->EIP;
// }

void CpuPanel::ReportBusy( bool isbusy )
{
    m_isbusy = isbusy;
}

void CpuPanel::OnMiddleDown( wxMouseEvent &event )
{
    const Processor *cpu = m_debugger->GetCurrentThread()->CPU();
    m_dad->OnPreExecSingleStep(cpu);
}
