#include "stdafx.h"
#include "cpupanel.h"


CpuPanel::CpuPanel( wxWindow *parent )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 400))
{
    InitUI();
}



void CpuPanel::InitUI()
{
//     this->SetFont(m_font);
//     wxPanel *leftPanel = InitLeftPanel();
//     wxPanel *rightPanel = InitRightPanel();
// 
//     wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
//     hsizer->Add(leftPanel, 1, wxEXPAND | wxALL);
//     hsizer->Add(rightPanel, 0, wxEXPAND | wxALL);
// 
//     SetSizer(hsizer);
    wxPanel *panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, 400));
}
