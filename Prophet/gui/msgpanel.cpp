#include "stdafx.h"
#include "msgpanel.h"
#include "protocol/message.h"

MessagePanel::MessagePanel( wxWindow *parent, ProEngine *engine )
    : SelectableScrolledControl(parent, wxSize(400, 200)), m_engine(engine)
{
    InitRender();
    m_message = NULL;
}

MessagePanel::~MessagePanel()
{

}

void MessagePanel::UpdateData( const Message *msg )
{
    m_message = msg;

    SetVirtualSize(m_width, m_message->Size() * m_lineHeight);
    Refresh();
}

void MessagePanel::InitRender()
{
    wxFont f = wxFont(g_config.GetInt("MessagePanel", "FontSize", 8),
        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false,
        g_config.GetString("MessagePanel", "FontName", "Lucida Console"));
    UpdateFont(f);

    m_currSelBrush  = wxBrush(wxColour(g_config.GetString("MessagePanel", "SelBgColor", "#c0c0c0")));
    m_widthIndex    = g_config.GetInt("MessagePanel", "WidthIndex", 45);
    m_widthHex      = g_config.GetInt("MessagePanel", "WidthHex", 30);

    m_width         = m_widthIndex + m_widthHex;
}

void MessagePanel::Draw( wxBufferedPaintDC &dc )
{
    if (m_message == NULL) {
        dc.SetBackground(*wxGREY_BRUSH);
        dc.Clear();
        return;
    }

    wxPoint viewStart = GetViewStart();
    wxSize clientSize = GetClientSize();
    const int istart = viewStart.y;
    const int iend = istart + clientSize.GetHeight() / m_lineHeight;

    for (int i = istart; i <= min(m_message->Size()-1, iend); i++) {
        DrawItem(dc, i);
    }

    dc.SetPen(*wxGREY_PEN);
    int px, py;
    GetScrollPixelsPerUnit(&px, &py);

    int lineX = m_widthIndex + VertLineOffset;
    int lineY0 = viewStart.y * py, lineY1 = lineY0 + clientSize.GetHeight();
    dc.DrawLine(lineX, lineY0, lineX, lineY1);
    lineX += m_widthHex;
    dc.DrawLine(lineX, lineY0, lineX, lineY1);

}

void MessagePanel::DrawItem( wxBufferedPaintDC &dc, int index )
{
    int h = m_lineHeight * index;
    wxRect rectToDraw(0, h, m_width, m_lineHeight);

    if (index == m_currSelIndex) {
        dc.SetBrush(m_currSelBrush);
        dc.DrawRectangle(rectToDraw);
    }

    const MessageByte &b = (*m_message)[index];
    int w = 0;
    dc.DrawText(wxString::Format("%04d", index), w, h);
    w += m_widthIndex;
    wxString hex;
    if (b.Data >= 0x20 && b.Data <= 0x7e) {
        hex = wxString::Format("%02x %c", b.Data, b.Data);
    } else {
        hex = wxString::Format("%02x", b.Data);
    }
    dc.DrawText(hex, w, h);
    w += m_widthHex;
}
