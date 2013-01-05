#include "stdafx.h"
#include "cpupanel.h"
#include "instruction.h"
#include "processor.h"

static bool InstCmp(const Disassembler::Inst &lhs, const Disassembler::Inst &rhs)
{
    return lhs.eip < rhs.eip;
}

CpuPanel::CpuPanel( wxWindow *parent )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 400)),
    m_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Lucida Console")
{
    InitUI();
    InitLogic();
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
    SetFont(m_font);
    wxBoxSizer *vsizer = new wxBoxSizer(wxVERTICAL);

//     m_list = new wxListCtrl(this, ID_CpuInstList, wxDefaultPosition, wxDefaultSize,
//         wxLC_REPORT | wxLC_VRULES | wxLC_SORT_ASCENDING | wxNO_BORDER);
//     
//     
// 
//     wxListItem col0;
//     col0.SetId(0);
//     col0.SetText("IP");
//     col0.SetWidth(70);
//     m_list->InsertColumn(0, col0);
// 
//     wxListItem col1;
//     col1.SetId(1);
//     col1.SetText("Disassembly");
//     col1.SetWidth(300);
//     m_list->InsertColumn(1, col1);

    m_list = new wxDataViewListCtrl(this, ID_CpuInstList, wxDefaultPosition, wxDefaultSize,
        wxDV_SINGLE | wxDV_VERT_RULES | wxNO_BORDER);

    m_list->AppendTextColumn("IP", wxDATAVIEW_CELL_INERT, 70);
    m_list->AppendTextColumn("Disassembly", wxDATAVIEW_CELL_INERT, 215);
    m_list->SetRowHeight(12);

//     wxVector<wxVariant> data;
//     data.push_back(wxVariant(true));
//     data.push_back(wxVariant("row1"));
//     m_list->AppendItem(data);
//     data.clear();
//     data.push_back( wxVariant(false) );
//     data.push_back( wxVariant("row 3") );
//     m_list->AppendItem(data);

    vsizer->Add(m_list, 1, wxALL | wxEXPAND);

    SetSizer(vsizer);
}

void CpuPanel::OnPreExecute( const Processor *cpu, const Instruction *inst )
{
    wxVector<wxVariant> data;
    data.push_back(wxVariant(wxString::Format("%08X", cpu->EIP)));
    data.push_back(wxVariant(inst->Main.CompleteInstr));
    //m_list->InsertItem(0, data);
    m_list->AppendItem(data);
    m_list->SelectRow(m_list->GetItemCount()-1);
}

void CpuPanel::InitLogic()
{
    //std::make_heap(m_instVector.begin(), m_instVector.end(), InstCmp);
}

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
// void CpuPanel::OnPtrChange( u32 addr )
// {
//     LxInfo("%08x\n", addr);
// }


