#include "stdafx.h"
#include "regpanel.h"
#include "processor.h"

RegPanel::RegPanel( wxWindow *parent )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(220, 400)),
    m_font(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, 
    false, "Lucida Console")
{
    InitUI();
}

const wxString RegPanel::RegLabels[] = {
    "Eax", "Ecx", "Edx", "Ebx", "Esp", "Ebp", "Esi", "Edi"
};

void RegPanel::InitUI()
{
    static const wxSize LabelSize = wxSize(30, 15);
    static const wxSize ValueSize = wxSize(60, 15);

    SetFont(m_font);
    //wxPanel *panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
    //SetBackgroundColour(wxColor("#D0D0D0"));

    wxBoxSizer *vsizer = new wxBoxSizer(wxVERTICAL);

    static const int Border = 2;
    for (int i = 0; i < RegCount; i++) {
        m_regs[i].Label = new wxStaticText(this, wxID_ANY, RegLabels[i], 
            wxDefaultPosition, LabelSize);
        m_regs[i].Value = new wxStaticText(this, wxID_ANY, "00000000",
            wxDefaultPosition, ValueSize);

        wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
        hsizer->Add(m_regs[i].Label, 0, wxLEFT | wxTOP, Border);
        hsizer->Add(m_regs[i].Value, 0, wxLEFT | wxTOP, Border);

        vsizer->Add(hsizer, 0, wxALL);
        //vsizer->AddSpacer(5);
    }

    wxStaticLine *line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 2));
    vsizer->Add(line, 0, wxEXPAND);

    m_eipLabel = new wxStaticText(this, wxID_ANY, "Eip", wxDefaultPosition, LabelSize);
    m_eipValue = new wxStaticText(this, wxID_ANY, "00000000", wxDefaultPosition, ValueSize);
    wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
    hsizer->Add(m_eipLabel, 0, wxLEFT | wxTOP, Border);
    hsizer->Add(m_eipValue, 0, wxLEFT | wxTOP, Border);
    vsizer->Add(hsizer, 0, wxALL);

    SetSizer(vsizer);
}

void RegPanel::SetValues( const Processor *cpu )
{
    for (int i = 0; i < RegCount; i++) {
        m_regs[i].Value->SetLabel(wxString::Format("%08X", cpu->GP_Regs[i].X32));
    }
    m_eipValue->SetLabel(wxString::Format("%08X", cpu->EIP));
}
