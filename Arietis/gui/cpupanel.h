#pragma once
 
#ifndef __ARIETIS_GUI_CPUPANEL_H__
#define __ARIETIS_GUI_CPUPANEL_H__
 
#include "Arietis.h"
#include "gui.h"
#include "../static/disassembler.h"
#include "parallel.h"
#include "custombase.h"

class CpuPanel : public SelectableScrolledControl {
public:
    CpuPanel(wxWindow *parent, AEngine *engine);
    ~CpuPanel();

    void        ShowCode(u32 addr);
    void        OnCurrentEipChange(u32 addr);
    void        OnDataUpdate(const InstSection *insts, const Processor *cpu);
    void        OnSelectionChange() override;

    void        OnRightDown(wxMouseEvent& event);
    void        OnPopupShowCurrInst(wxCommandEvent &event);
    void        OnPopupTaintReg(wxCommandEvent &event);
    void        OnPopupTrackMemory(wxCommandEvent &event);

    u32         GetCurrentEip() const { return m_currEip; }
    u32         GetSelectedEip() const { return m_currSelEip; }

private:
    void        InitRender();
    void        InitMenu();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawInst(wxBufferedPaintDC &dc, const InstPtr inst, int index);
    void        DrawJumpIcon(wxBufferedPaintDC &dc, const InstPtr inst, int index);
    void        DrawJumpLines(wxBufferedPaintDC &dc, int istart, int iend);
    wxPoint     GetCurrentScrolledPos() const;
    int         CalcJumpLineWidth(int idx1, int idx2) const;
    void        TrackMemory(u32 instEip);
private:
    wxMenu *    m_popup;
    AEngine *   m_engine;
    const Processor *   m_cpu;
    
    /* render */
    int         m_widthIp;
    int         m_widthDisasm;
    int         m_widthInfo;
    int         m_width;
    int         m_height;
    int         m_currIndex;
    u32         m_currEip;
    u32         m_currSelEip;
    int         m_minDistanceToBottom;
    bool        m_showTargetNameInstead;
    int         m_jumpLineWidthMax;
    int         m_jumpLineWidthMin;
    int         m_jumpLineInstDistMax;

    wxBrush     m_curlineBrush;
    wxBrush     m_sellineBrush;
    wxBrush     m_zerolineBrush;
    wxColour    m_callRetColor;
    wxColour    m_jumpColor;

    const InstSection *     m_insts;
    std::map<int, int>      m_procEntryEnd;
};
 
#endif // __ARIETIS_GUI_CPUPANEL_H__