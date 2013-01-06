#include "stdafx.h"
#include "contextpanel.h"
#include "processor.h"

ContextPanel::ContextPanel( wxWindow *parent )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(220, 400))
{
    m_mutex = MutexCS::Create();
    InitRender();

    Bind(wxEVT_PAINT, &ContextPanel::OnPaint, this, wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &ContextPanel::OnEraseBackground, this, wxID_ANY);
}


ContextPanel::~ContextPanel()
{
    MutexCS::Destroy(m_mutex);
}

const wxString ContextPanel::RegLabels[] = {
    "Eax", "Ecx", "Edx", "Ebx", "Esp", "Ebp", "Esi", "Edi"
};

void ContextPanel::InitRender()
{
    m_font = wxFont(g_config.GetInt("ContextPanel", "FontSize", 9), 
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, 
        g_config.GetString("ContextPanel", "FontName", "Lucida Console"));

    SetFont(m_font);
    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_lineHeight = dc.GetFontMetrics().height;

    m_bgBrush = wxBrush(wxColour(g_config.GetString("ContextPanel", "BgColor", "#e0e0e0")));

    m_widthRegName = g_config.GetInt("ContextPanel", "WidthRegName", 40);
    m_widthRegValue = g_config.GetInt("ContextPanel", "WidthRegValue", 80);

    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void ContextPanel::OnPaint( wxPaintEvent &event )
{
    wxBufferedPaintDC dc(this);
    dc.SetBackground(m_bgBrush);
    dc.Clear();

    Draw(dc);
}

void ContextPanel::Draw( wxBufferedPaintDC &dc )
{
    MutexCSLock lock(*m_mutex);
    for (int i = 0; i < InstContext::RegCount; i++) {
        dc.DrawText(RegLabels[i], 0, i * m_lineHeight);
        dc.DrawText(wxString::Format("%08X", m_data.regs[i]), 
            m_widthRegName, i * m_lineHeight);
    }
}

void ContextPanel::OnEraseBackground( wxEraseEvent& event )
{

}

void ContextPanel::UpdateData( const InstContext &ctx )
{
    {
        MutexCSLock lock(*m_mutex);
        m_data = ctx;
    }
    Refresh();
}

