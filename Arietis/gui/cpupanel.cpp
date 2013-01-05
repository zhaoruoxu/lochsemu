#include "stdafx.h"
#include "cpupanel.h"
#include "instruction.h"

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

    m_list = new wxListCtrl(this, ID_CpuInstList, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_VRULES | wxLC_SORT_ASCENDING | wxNO_BORDER);
    
    wxBoxSizer *vsizer = new wxBoxSizer(wxVERTICAL);

    wxListItem col0;
    col0.SetId(0);
    col0.SetText("IP");
    col0.SetWidth(70);
    m_list->InsertColumn(0, col0);

    wxListItem col1;
    col1.SetId(1);
    col1.SetText("Disassembly");
    col1.SetWidth(300);
    m_list->InsertColumn(1, col1);

    vsizer->Add(m_list, 1, wxALL | wxEXPAND);

    SetSizer(vsizer);
}


void CpuPanel::InitLogic()
{
    std::make_heap(m_instVector.begin(), m_instVector.end(), InstCmp);
}

void CpuPanel::OnInstDisasm( const Disassembler::InstVector &insts )
{
//     for (auto &inst : insts) {
//         m_instVector.push_back(inst);
//         std::push_heap(m_instVector.begin(), m_instVector.end(), InstCmp);
//     }
//     LxInfo("%d instructions added to debugger\n", insts.size());
// 
//     std::sort_heap(m_instVector.begin(), m_instVector.end(), InstCmp);
// 
//     m_list->DeleteAllItems();

    int id = 0;
    for (auto &inst : insts) {
        wxListItem item;
        item.SetId(id);
        item.SetText("shit");
        item.SetData(inst.eip);

        m_list->InsertItem(item);
        m_list->SetItem(id, 0, wxString::Format("%08X", inst.eip));
        m_list->SetItem(id, 1, wxString::Format("%s", inst.instPtr->Main.CompleteInstr));
        id++;
    }
    m_list->SortItems(InstListItemCmp, 0);
}

void CpuPanel::OnPtrChange( u32 addr )
{
    LxInfo("%08x\n", addr);
}

void CpuPanel::OnPreExecute( const Processor *cpu, const Instruction *inst )
{

}
