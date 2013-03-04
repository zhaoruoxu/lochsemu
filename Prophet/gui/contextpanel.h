#pragma once

#ifndef __PROPHET_GUI_REGPANEL_H__
#define __PROPHET_GUI_REGPANEL_H__

#include "Prophet.h"
#include "gui.h"
#include "parallel.h"
#include "engine.h"

class ContextPanel : public wxPanel {
public:
    ContextPanel(wxWindow *parent);
    ~ContextPanel();

    void        OnPaint(wxPaintEvent &event);
    void        OnEraseBackground(wxEraseEvent& event);
    void        UpdateData(const InstContext *ctx, const char *dataDesc);
private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc);
private:
    MutexCS     m_mutex;
    InstContext m_data;
    wxString    m_dataDesc;

    wxFont      m_font;
    int         m_lineHeight;
    int         m_widthRegName;
    int         m_widthRegValue;
    int         m_widthTaint;
    int         m_widthFlagValue;

    wxBrush     m_bgBrush;
};

#endif // __PROPHET_GUI_REGPANEL_H__
