#pragma once

#ifndef __ARIETIS_GUI_CUSTOMBASE_H__
#define __ARIETIS_GUI_CUSTOMBASE_H__

#include "Arietis.h"
#include "gui.h"

class CustomScrolledControl : public wxScrolledWindow {
public:
    CustomScrolledControl(wxWindow *parent, const wxSize &size);
    
    void        OnPaint(wxPaintEvent &event);
    void        OnEraseBackground(wxEraseEvent &event);
    virtual void    UpdateFont(const wxFont &font);
    virtual void    Draw(wxBufferedPaintDC &dc) = 0;

protected:
    wxFont      m_font;
    int         m_lineHeight;
    wxBrush     m_bgBrush;
};

#endif // __ARIETIS_GUI_CUSTOMBASE_H__
