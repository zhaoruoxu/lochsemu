#pragma once
 
#ifndef __PROPHET_GUI_MSGPANEL_H__
#define __PROPHET_GUI_MSGPANEL_H__
 
#include "prophet.h"
#include "gui.h"
#include "custombase.h"

class MessagePanel : public SelectableScrolledControl {
public:
    MessagePanel(wxWindow *parent, ProEngine *engine);
    ~MessagePanel();

    void        UpdateData(const Message *msg);
private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawItem(wxBufferedPaintDC &dc, int index);

private:
    ProEngine * m_engine;
    const Message * m_message;

    int         m_widthIndex;
    int         m_widthHex;
    int         m_width;

    wxBrush     m_currSelBrush;
};

#endif // __PROPHET_GUI_MSGPANEL_H__