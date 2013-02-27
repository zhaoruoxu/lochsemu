#include "stdafx.h"
#include "custombase.h"


CustomScrolledControl::CustomScrolledControl( wxWindow *parent, const wxSize &size )
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, size, wxVSCROLL)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &CustomScrolledControl::OnPaint, this ,wxID_ANY);
    Bind(wxEVT_ERASE_BACKGROUND, &CustomScrolledControl::OnEraseBackground, this, wxID_ANY);
    Bind(wxEVT_ENTER_WINDOW, &CustomScrolledControl::OnMouseEnter, this, wxID_ANY);
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

void CustomScrolledControl::OnMouseEnter( wxMouseEvent &event )
{
    SetFocus();
}

SelectableScrolledControl::SelectableScrolledControl( wxWindow *parent, const wxSize &size )
    : CustomScrolledControl(parent, size)
{
    Bind(wxEVT_LEFT_DOWN,   &SelectableScrolledControl::OnLeftDown,     this, wxID_ANY);
    Bind(wxEVT_LEFT_UP,     &SelectableScrolledControl::OnLeftUp,       this, wxID_ANY);
    Bind(wxEVT_MOTION,      &SelectableScrolledControl::OnMouseMove,    this, wxID_ANY);
    Bind(wxEVT_LEAVE_WINDOW,&SelectableScrolledControl::OnMouseLeave,   this, wxID_ANY);

    m_currSelIndex  = -1;
    m_isLeftDown    = false;
}

void SelectableScrolledControl::OnLeftDown( wxMouseEvent &event )
{
    wxPoint p       = event.GetPosition();
    wxPoint ps      = GetViewStart();
    m_currSelIndex  = ps.y + p.y / m_lineHeight;
    m_isLeftDown    = true;

    Refresh();
    OnSelectionChange();
}

void SelectableScrolledControl::OnLeftUp( wxMouseEvent &event )
{
    m_isLeftDown    = false;
}

void SelectableScrolledControl::OnMouseMove( wxMouseEvent &event )
{
    if (!m_isLeftDown) return;
    wxPoint p       = event.GetPosition();
    wxPoint ps      = GetViewStart();
    int index       = ps.y + p.y / m_lineHeight;
    if (index != m_currSelIndex) {
        m_currSelIndex = index;
        Refresh();
        OnSelectionChange();
    }
}

void SelectableScrolledControl::OnMouseLeave( wxMouseEvent &event )
{
    m_isLeftDown    = false;
}
