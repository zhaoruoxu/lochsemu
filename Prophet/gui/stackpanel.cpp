#include "stdafx.h"
#include "stackpanel.h"
#include "parallel.h"
#include "engine.h"
#include "cpupanel.h"
#include "mainframe.h"
#include "stack.h"

StackPanel::StackPanel( wxWindow *parent, ProphetFrame *dad, ProEngine *engine )
    : SelectableScrolledControl(parent, wxSize(200, 200)), m_dad(dad), m_engine(engine)
{
    InitRender();

    Bind(wxEVT_LEFT_DCLICK,     &StackPanel::OnLeftDoubleClick,     this, wxID_ANY);
    m_cpu = NULL;
    m_stackSize = 0;
    m_prevEsp = 0;
}

StackPanel::~StackPanel()
{

}

void StackPanel::InitRender()
{
    wxFont f = wxFont(g_config.GetInt("StackPanel", "FontSize", 9),
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false,
        g_config.GetString("CallStackPanel", "FontName", "Lucida Console"));
    UpdateFont(f);

    m_widthAddr     = g_config.GetInt("StackPanel", "WidthAddress", 75);
    m_widthVal      = g_config.GetInt("StackPanel", "WidthVal", 75);
    m_widthDesc     = g_config.GetInt("StackPanel", "WidthDesc", 300);
    m_width         = m_widthAddr + m_widthDesc + m_widthVal;

    m_currSelBrush  = wxBrush(wxColour("#c0c0c0"));
}

void StackPanel::Draw( wxBufferedPaintDC &dc )
{
    if (m_engine == NULL) return;
    if (m_cpu == NULL || m_stackSize == 0) return;

    Stack *s = m_cpu->Thr()->GetStack();

    wxPoint viewStart   = GetViewStart();
    wxSize clientSize   = GetClientSize();
    const int istart    = viewStart.y;
    const int iend      = istart + clientSize.GetHeight() / m_lineHeight;
    for (int i = istart; i <= min(m_stackSize-1, iend); i++)
        DrawItem(dc, s, i);

    dc.SetPen(*wxGREY_PEN);
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);

    int lineX = m_widthAddr + VertLineOffset;
    int lineY0 = viewStart.y * py, lineY1 = lineY0 + clientSize.GetHeight();
    dc.DrawLine(lineX, lineY1, lineX, lineY0);
    lineX += m_widthVal;
    dc.DrawLine(lineX, lineY1, lineX, lineY0);
    lineX += m_widthDesc;
    dc.DrawLine(lineX, lineY1, lineX, lineY0);
}

void StackPanel::DrawItem( wxBufferedPaintDC &dc, Stack *s, int index )
{
    u32 addr = m_cpu->ESP + index * 4;
    u32 val = m_cpu->MemRead32(addr);

    int h = m_lineHeight * index;
    wxRect rectToDraw(0, h, m_width, m_lineHeight);
    if (index == m_currSelIndex) {
        dc.SetBrush(m_currSelBrush);
        dc.DrawRectangle(rectToDraw);
    }

    int w = 0;
    dc.DrawText(wxString::Format("%08x", addr), w, h);
    w += m_widthAddr;
    
    wxString desc;
    dc.DrawText(wxString::Format("%08x", val), w, h);
    w += m_widthVal;

    Section *sec = m_cpu->Mem->GetSection(val);
    if (sec && (sec->GetDesc().Desc == ".text" || sec->GetDesc().Desc == ".code")) {
        uint nModule = m_cpu->GetModule(val);
        const ModuleInfo *mInfo = m_cpu->Proc()->GetModuleInfo(nModule);
        wxString desc;
//         if (m_isPrevCall && m_callinfo.find(addr) != m_callinfo.end()) {
//             desc = wxString::Format("from %s to %s.%s", mInfo->Name, m_callinfo[addr].Module,
//                 m_callinfo[addr].Func);
//         } else {
            desc = wxString::Format("%s.%08x", mInfo->Name, val);
/*        }*/
        dc.DrawText(desc, w, h);
        w += m_widthDesc;
    }
}

void StackPanel::UpdateData(const Processor *cpu)
{
    m_cpu = cpu;
    Stack *s = m_cpu->Thr()->GetStack();
    m_stackSize = (s->Top() - m_cpu->ESP) / 4;

    SetVirtualSize(m_width, m_lineHeight * m_stackSize);
    Refresh();
}

void StackPanel::OnLeftDoubleClick( wxMouseEvent &event )
{
    if (m_currSelIndex < 0 || m_currSelIndex >= m_stackSize)
        return;

    u32 addr = m_cpu->MemRead32(m_cpu->ESP + m_currSelIndex * 4);
    Section *sec = m_cpu->Mem->GetSection(addr);
    if (sec == NULL) {
        wxMessageBox(wxString::Format("invalid address: %08x", addr));
        return;
    }
    if (sec->GetDesc().Desc != ".text" && sec->GetDesc().Desc != ".code") {
        wxMessageBox("Section cannot be shown: " + sec->GetDesc().Desc);
        return;
    }

    m_dad->GetCpuPanel()->ShowCode(addr);
}

void StackPanel::OnPostExecute( PostExecuteEvent &event )
{
//     m_isPrevCall = Instruction::IsCall(event.Inst);
//     m_prevEsp = event.Cpu->ESP;
//     if (!m_isPrevCall) return;
//     InstPtr pinst = m_engine->GetDisassembler()->GetInst(event.Cpu->GetPrevEip());
//     u32 target = pinst->Target;
//     if (target == (u32) -1) return;
//     StackCallInfo scinfo;
//     scinfo.Module = pinst->TargetModuleName;
//     scinfo.Func = pinst->TargetFuncName;
//     m_callinfo[m_prevEsp] = scinfo;
    
}
