#include "stdafx.h"
#include "contextpanel.h"
#include "processor.h"

ContextPanel::ContextPanel( wxWindow *parent )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 200))
{
    //m_mutex = MutexCS::Create();
    InitRender();

    Bind(wxEVT_PAINT, &ContextPanel::OnPaint, this, wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &ContextPanel::OnEraseBackground, this, wxID_ANY);
    m_dataDesc = "Invalid";
}


ContextPanel::~ContextPanel()
{
    //MutexCS::Destroy(m_mutex);
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
    MutexCSLock lock(m_mutex);
    int h = 0;

    dc.DrawText(wxString::Format("----  %s  ----", m_dataDesc), 0, 0);
    h += m_lineHeight;

    if (!m_data.inst) return;

    dc.DrawText("Disassembly:", 0, h);
    h += m_lineHeight;
    dc.DrawText(wxString::Format("[ %s ]", m_data.inst->Main.CompleteInstr), 0, h);
    h += m_lineHeight;

    if (m_data.inst->Entry != -1) {
        dc.DrawText(wxString::Format("Entry: %08X", m_data.inst->Entry), 0, h);
        h += m_lineHeight;
    }
    if (m_data.inst->Target != -1) {
        dc.DrawText(wxString::Format("Target: %08X", m_data.inst->Target), 0, h);
        h += m_lineHeight;
    }

    h += m_lineHeight;

    wxString mod = wxString::Format("Module [%s] at %08X", m_data.moduleName, m_data.moduleImageBase);
    dc.DrawText(mod, 0, h);
    h += m_lineHeight * 2;

    for (int i = 0; i < InstContext::RegCount; i++) {
        dc.DrawText(RegLabels[i], 0, h);
        dc.DrawText(wxString::Format("%08X", m_data.regs[i]), m_widthRegName, h);
        h += m_lineHeight;
    }


    h += m_lineHeight;
    dc.DrawText("Eip", 0, h);
    dc.DrawText(wxString::Format("%08X", m_data.inst->Eip), m_widthRegName, h);
    h += m_lineHeight * 2;

    dc.DrawText("  TE MO RE SE", m_widthRegName, h);
    h += m_lineHeight;

    if (m_data.inst) {
        u8p pFlag = (u8p) &m_data.inst->Main.Inst.Flags;
        for (int i = 0; i < InstContext::FlagCount; i++) {
            dc.DrawText(wxString::Format(" %s", InstContext::FlagNames[i]), 0, h);
            wxString flag = wxString::Format("%d  %d  %d  %d  %d", m_data.flags[i],
                (*pFlag & TE_) != 0, (*pFlag & MO_) != 0, (*pFlag & RE_) != 0, (*pFlag & SE_) != 0);
            pFlag++;
            dc.DrawText(flag, m_widthRegName, h);
            h += m_lineHeight;
        }
        h += m_lineHeight * 2;
    }

}

void ContextPanel::OnEraseBackground( wxEraseEvent& event )
{

}

void ContextPanel::UpdateData( const InstContext &ctx, const char *dataDesc )
{
    {
        MutexCSLock lock(m_mutex);
        m_data = ctx;
        m_dataDesc = dataDesc;
    }
    Refresh();
}

