#include "stdafx.h"
#include "cpupanel.h"


CpuPanel::CpuPanel( wxWindow *parent )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(600, -1))
{
    InitUI();
}

void CpuPanel::InitUI()
{
    wxPanel *leftPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, -1));
    wxPanel *rightPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, -1));

    leftPanel->SetBackgroundColour(wxColor("#ff8080"));
    rightPanel->SetBackgroundColour(wxColor("#8080ff"));

    wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
    hsizer->Add(leftPanel, 1, wxEXPAND | wxALL);
    hsizer->Add(rightPanel, 0, wxEXPAND | wxALL);

    SetSizer(hsizer);
}
