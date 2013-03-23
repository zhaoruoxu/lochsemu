#pragma once
 
#ifndef __PROPHET_GUI_CPUPANEL_H__
#define __PROPHET_GUI_CPUPANEL_H__
 
#include "prophet.h"
#include "gui.h"
#include "../static/disassembler.h"
#include "parallel.h"
#include "custombase.h"

class CpuPanel : public SelectableScrolledControl {
public:
    CpuPanel(wxWindow *parent, ProphetFrame *dad, ProEngine *engine);
    ~CpuPanel();

    void        ShowCode(u32 addr);
    void        OnCurrentEipChange(u32 addr);
    void        OnSelectionChange() override;
    void        ReportBusy(bool isbusy);

    void        OnRightDown(wxMouseEvent& event);
    void        OnPopupShowCurrInst(wxCommandEvent &event);
    void        OnPopupShowTargetInst(wxCommandEvent &event);
    void        OnPopupTaintReg(wxCommandEvent &event);
    void        OnPopupTrackMemory(wxCommandEvent &event);

    u32         GetCurrentEip() const { return m_currEip; }
    u32         GetSelectedEip() const { return m_currSelEip; }

    void        OnPreExecute(PreExecuteEvent &event);
    void        OnProcessPostLoad(ProcessPostLoadEvent &event);

private:
    void        OnDataUpdate(u32 addr);
    void        ScrollProperly(int index);
    void        InitRender();
    void        InitMenu();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawInst(wxBufferedPaintDC &dc, const InstPtr inst, int index);
    void        DrawJumpLine(wxBufferedPaintDC &dc, const InstPtr inst, int index);
    void        DrawJumpIcon(wxBufferedPaintDC &dc, const InstPtr inst, int index);
    wxPoint     GetCurrentScrolledPos() const;
    void        TrackMemory(u32 instEip);
private:
    bool        m_isbusy;
    ProphetFrame *  m_dad;
    wxMenu *    m_popup;
    ProEngine *   m_engine;
    const Processor *   m_cpu;
    Disassembler *  m_disasm;
    const InstSection *     m_insts;

    
    /* render */
    int         m_widthIp;
    int         m_widthDisasm;
    int         m_widthInfo;
    int         m_width;
    int         m_height;

    int         m_currIndex;
    u32         m_currEip;
    u32         m_currSelEip;
    int         m_minDistanceToEdge;
    bool        m_showTargetNameInstead;

    wxBrush     m_curlineBrush;
    wxBrush     m_sellineBrush;
    wxBrush     m_zerolineBrush;
    wxColour    m_callRetColor;
    wxColour    m_jumpColor;
};
    
 
#endif // __PROPHET_GUI_CPUPANEL_H__