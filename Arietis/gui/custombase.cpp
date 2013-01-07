#include "stdafx.h"
#include "custombase.h"


CustomScrolledControl::CustomScrolledControl( wxWindow *parent, const wxSize &size )
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, size, wxVSCROLL)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &CustomScrolledControl::OnPaint, this ,wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &CustomScrolledControl::OnEraseBackground, this, wxID_ANY);
    m_bgBrush = *wxWHITE_BRUSH;
}

void CustomScrolledControl::OnPaint( wxPaintEvent &event )
{
    wxBufferedPaintDC dc(this);
    DoPrepareDC(dc);
    dc.SetBackground(m_bgBrush);
    dc.Clear();

    Draw(dc);
}

void CustomScrolledControl::OnEraseBackground( wxEraseEvent &event )
{

}

void CustomScrolledControl::UpdateFont( const wxFont &font )
{
    m_font = font;
    SetFont(m_font);
    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_lineHeight = dc.GetFontMetrics().height;
    SetScrollRate(1, m_lineHeight);
}
