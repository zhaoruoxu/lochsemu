#pragma once
 
#ifndef __PROPHET_GUI_MSGPANEL_H__
#define __PROPHET_GUI_MSGPANEL_H__
 
#include "prophet.h"
#include "gui.h"
#include "custombase.h"
#include "protocol/message.h"

class MessagePanel : public SelectableScrolledControl {
public:
    MessagePanel(wxWindow *parent, ProphetFrame *dad, ProEngine *engine);
    ~MessagePanel();

    void        UpdateData(const Message *msg);
private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawItem(wxBufferedPaintDC &dc, int index);

private:
    ProphetFrame *  m_dad;
    ProEngine * m_engine;
    const Message * m_message;

    int         m_widthIndex;
    int         m_widthAddr;
    int         m_widthHex;
    int         m_widthFormat;
    int         m_widthDesc;
    int         m_width;

    wxBrush     m_currSelBrush;
    wxBrush     m_formatBrushes[FieldFormat::Total];
};

#endif // __PROPHET_GUI_MSGPANEL_H__