#pragma once

#ifndef __PROPHET_GUI_CUSTOMBASE_H__
#define __PROPHET_GUI_CUSTOMBASE_H__

#include "prophet.h"
#include "gui.h"

class CustomScrolledControl : public wxScrolledWindow {
public:
    CustomScrolledControl(wxWindow *parent, const wxSize &size);
    
    void        OnPaint(wxPaintEvent &event);
    void        OnEraseBackground(wxEraseEvent &event);
    void        OnMouseEnter(wxMouseEvent &event);
    virtual void    UpdateFont(const wxFont &font);
    virtual void    Draw(wxBufferedPaintDC &dc) = 0;

    static const int VertLineOffset = -2;

protected:
    wxFont      m_font;
    int         m_lineHeight;
    wxBrush     m_bgBrush;
};

class SelectableScrolledControl : public CustomScrolledControl {
public:
    SelectableScrolledControl(wxWindow *parent, const wxSize &size);

    void        OnLeftDown(wxMouseEvent &event);
    void        OnLeftUp(wxMouseEvent &event);
    void        OnMouseMove(wxMouseEvent &event);
    void        OnMouseLeave(wxMouseEvent &event);

    virtual void    OnSelectionChange() {}
protected:
    int         m_currSelIndex;
    bool        m_isLeftDown;
};

#endif // __PROPHET_GUI_CUSTOMBASE_H__
