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
}

int wxCALLBACK InstListItemCmp(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    if (item1 > item2)
        return 1;
    if (item1 < item2)
        return -1;
    return 0;
}

void CpuPanel::InitUI()
{
    wxBoxSizer *vsizer = new wxBoxSizer(wxVERTICAL);

    m_list = new wxDataViewListCtrl(this, ID_CpuInstList, wxDefaultPosition, wxDefaultSize,
        wxDV_SINGLE | wxDV_VERT_RULES | wxNO_BORDER);

    m_list->AppendTextColumn("IP", wxDATAVIEW_CELL_INERT, 70);
    m_list->AppendTextColumn("Disassembly", wxDATAVIEW_CELL_INERT, 300);
    m_list->SetRowHeight(12);

    vsizer->Add(m_list, 1, wxALL | wxEXPAND);

    SetSizer(vsizer);
}


void CpuPanel::InitLogic()
{
    //std::make_heap(m_instVector.begin(), m_instVector.end(), InstCmp);
}

void CpuPanel::InitRender()
{
    m_widthIp = g_config.GetInt("CpuPanel", "WidthIp", 70);
    m_widthDisasm = g_config.GetInt("CpuPanel", "WidthDisasm", 215);
    m_minDistanceToBottom = g_config.GetInt("CpuPanel", "MinDistanceToBottom", 7);

    m_font = wxFont(g_config.GetInt("CpuPanel", "FontSize", 8), wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("CpuPanel", "FontName", "Lucida Console"));
    m_curlineBrush = wxBrush(wxColor(g_config.GetString("CpuPanel", "CurrentLineColor", "#00FF00")));

    m_width = m_widthIp + m_widthDisasm;

    SetFont(m_font);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_fontMetrics = dc.GetFontMetrics();
    m_currIndex = 0;
    SetScrollRate(1, m_fontMetrics.height);
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
    int idxStart = (p.y - m_fontMetrics.height) / m_fontMetrics.height;
    int idxEnd = (p.y + cs.GetHeight() + m_fontMetrics.height) / m_fontMetrics.height;

    /* instructions */
    int index = 0;
    for (auto &inst : *m_insts) {
        if (idxStart <= index && index <= idxEnd)
            DrawInst(dc, inst.second, index);
        index++;
    }

    /* clear unwanted */
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(p.x + m_widthIp + m_widthDisasm + VertLineOffset, p.y, 
        m_width, p.y + cs.GetHeight());

    dc.SetPen(*wxGREY_PEN);

    /* vertical lines */
    int lineX = p.x + m_widthIp + VertLineOffset;
    int lineY0 = p.y, lineY1 = p.y + cs.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthDisasm;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    /* highlight */
    //HighlightCurrentInst(dc);
}

void CpuPanel::DrawInst( wxBufferedPaintDC &dc, const Disassembler::Inst &inst, int index )
{
    wxRect rectToDraw(0, m_fontMetrics.height * index, m_width, m_fontMetrics.height);
    wxRect rectScrolled = rectToDraw;
    CalcScrolledPosition(rectToDraw.x, rectToDraw.y, &rectScrolled.x, &rectScrolled.y);
    if (!IsExposed(rectScrolled)) return;

    if (index == m_currIndex) {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_curlineBrush);
        dc.DrawRectangle(rectToDraw);
        dc.SetBrush(*wxWHITE_BRUSH);
    }
    dc.DrawText(wxString::Format("%08X", inst.eip), 0, m_fontMetrics.height * index);
    dc.DrawText(wxString::Format("%s", inst.instPtr->Main.CompleteInstr),
        m_widthIp, m_fontMetrics.height * index);
}

void CpuPanel::HighlightCurrentInst( wxBufferedPaintDC &dc )
{
    wxRect r(0, m_fontMetrics.height * m_currIndex, m_width, m_fontMetrics.height);
    CalcScrolledPosition(r.x, r.y, &r.x, &r.y);
    if (!IsExposed(r)) return;

    wxBrush oldBrush = dc.GetBrush();
    wxPen   oldPen = dc.GetPen();
    dc.SetBrush(*wxRED_BRUSH);
    //dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(r);
    dc.SetBrush(oldBrush);
    //dc.SetPen(oldPen);
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
    SetScrollRate(10, m_fontMetrics.height);

    m_height = m_fontMetrics.height * insts->size();
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
    int h = cs.GetHeight() / m_fontMetrics.height;
    if (m_currIndex < p.y) {
        Scroll(0, m_currIndex);
    } else if (m_currIndex - p.y > h - m_minDistanceToBottom) {
        Scroll(0, m_currIndex - (h - m_minDistanceToBottom));
    }

    Refresh();
}

// wxCoord CpuPanel::OnGetRowHeight( size_t row ) const
// {
//     return 8;
// }
// 
// wxCoord CpuPanel::OnGetColumnWidth( size_t column ) const
// {
//     return 100;
// }

// void CpuPanel::OnInstDisasm( const Disassembler::InstVector &insts )
// {
// //     for (auto &inst : insts) {
// //         m_instVector.push_back(inst);
// //         std::push_heap(m_instVector.begin(), m_instVector.end(), InstCmp);
// //     }
// //     LxInfo("%d instructions added to debugger\n", insts.size());
// // 
// //     std::sort_heap(m_instVector.begin(), m_instVector.end(), InstCmp);
// // 
// //     m_list->DeleteAllItems();
// 
//     int id = 0;
//     for (auto &inst : insts) {
//         wxListItem item;
//         item.SetId(id);
//         item.SetText("shit");
//         item.SetData(inst.eip);
// 
//         m_list->InsertItem(item);
//         m_list->SetItem(id, 0, wxString::Format("%08X", inst.eip));
//         m_list->SetItem(id, 1, wxString::Format("%s", inst.instPtr->Main.CompleteInstr));
//         id++;
//     }
//     m_list->SortItems(InstListItemCmp, 0);
// }
// 



