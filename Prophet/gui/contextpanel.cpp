#include "stdafx.h"
#include "contextpanel.h"
#include "processor.h"

ContextPanel::ContextPanel( wxWindow *parent, ProphetFrame *dad )
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 200)),
    m_dad(dad)
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

    m_widthRegName  = g_config.GetInt("ContextPanel", "WidthRegName", 40);
    m_widthRegValue = g_config.GetInt("ContextPanel", "WidthRegValue", 80);
    m_widthTaint    = g_config.GetInt("ContextPanel", "WidthTaint", 64);
    m_widthFlagValue    = g_config.GetInt("ContextPanel", "WidthFlagValue", 110);

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

    if (!m_data.Inst) return;

    dc.DrawText("Disassembly:", 0, h);
    h += m_lineHeight;
    dc.DrawText(wxString::Format("[ %s ]", m_data.Inst->Main.CompleteInstr), 0, h);
    h += m_lineHeight;

    if (m_data.Inst->Entry != -1) {
        dc.DrawText(wxString::Format("Entry: %08X", m_data.Inst->Entry), 0, h);
        h += m_lineHeight;
    }
    if (m_data.Inst->Target != -1) {
        dc.DrawText(wxString::Format("Target: %08X", m_data.Inst->Target), 0, h);
        h += m_lineHeight;
    }

    h += m_lineHeight;

    wxString mod = wxString::Format("Module [%s] at %08X", m_data.ModuleName, m_data.ModuleImageBase);
    dc.DrawText(mod, 0, h);
    h += m_lineHeight * 2;

    for (int i = 0; i < InstContext::RegCount; i++) {
        dc.DrawText(InstContext::RegNames[i], 0, h);
        dc.DrawText(wxString::Format("%08X", m_data.Regs[i]), m_widthRegName, h);
        int w = m_widthRegName + m_widthRegValue;
        DrawTaint(dc, m_data.RegTaint[i], wxRect(w, h, m_widthTaint, m_lineHeight));

        h += m_lineHeight;
    }


    h += m_lineHeight;
    dc.DrawText("Eip", 0, h);
    dc.DrawText(wxString::Format("%08X", m_data.Inst->Eip), m_widthRegName, h);
    DrawTaint(dc, m_data.EipTaint, wxRect(m_widthRegName + m_widthRegValue,
        h, m_widthTaint, m_lineHeight));
    h += m_lineHeight * 2;

    dc.DrawText("  TE MO RE SE", m_widthRegName, h);
    h += m_lineHeight;

    for (int i = 0; i < InstContext::FlagCount; i++) {
        dc.DrawText(wxString::Format(" %s", InstContext::FlagNames[i]), 0, h);
        wxString flag = wxString::Format("%d  %d  %d  %d  %d", m_data.Flags[i],
            IsFlagTested(m_data.Inst, i), IsFlagModified(m_data.Inst, i),
            IsFlagReset(m_data.Inst, i), IsFlagSet(m_data.Inst, i));
        dc.DrawText(flag, m_widthRegName, h);
        DrawTaint(dc, m_data.FlagTaint[i],
            wxRect(m_widthRegName + m_widthFlagValue, h, m_widthTaint, m_lineHeight));
        h += m_lineHeight;
    }
    h += m_lineHeight;

    if (Instruction::IsConditionalJump(m_data.Inst)) {
        dc.DrawText(m_data.JumpTaken ? "Jump is taken" : "Jump is NOT taken", 0, h);
    }

}

void ContextPanel::OnEraseBackground( wxEraseEvent& event )
{

}

void ContextPanel::UpdateData( const InstContext *ctx, const char *dataDesc )
{
    {
        MutexCSLock lock(m_mutex);
        m_data = *ctx;
        m_dataDesc = dataDesc;
    }
    Refresh();
}

