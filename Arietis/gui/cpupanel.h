#pragma once
 
#ifndef __ARIETIS_GUI_CPUPANEL_H__
#define __ARIETIS_GUI_CPUPANEL_H__
 
#include "Arietis.h"
#include "gui.h"
#include "../static/disassembler.h"
#include "parallel.h"
#include "custombase.h"

class CpuPanel : public CustomScrolledControl {
public:
    CpuPanel(wxWindow *parent);
    ~CpuPanel();

//     void        OnInstDisasm(const Disassembler::InstVector &insts);
    void        OnPtrChange(u32 addr);
    void        OnDataUpdate(const Disassembler::InstDisasmMap *insts);
    //void        OnPreExecute(const Processor *cpu, const Instruction *inst);


    void        OnLeftDown(wxMouseEvent& event);
    void        OnLeftUp(wxMouseEvent& event);
    void        OnRightDown(wxMouseEvent& event);
    void        OnMouseLeave(wxMouseEvent& event);
    void        OnMouseMove(wxMouseEvent& event);
    //wxCoord     OnGetRowHeight(size_t row) const override;
    //wxCoord     OnGetColumnWidth(size_t column) const override;
private:
    void        InitLogic();
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawInst(wxBufferedPaintDC &dc, const Disassembler::Inst &inst, int index);
    void        DrawJumpIcon(wxBufferedPaintDC &dc, const Disassembler::Inst &inst, int index);
    void        DrawJumpLines(wxBufferedPaintDC &dc, int istart, int iend);
    wxPoint     GetCurrentScrolledPos() const;
    int         CalcJumpLineWidth(int idx1, int idx2) const;
private:
    
    /* render */
    int         m_widthIp;
    int         m_widthDisasm;
    int         m_widthInfo;
    int         m_width;
    int         m_height;
    int         m_currIndex;
    u32         m_currEip;
    int         m_currSelIndex;
    u32         m_currSelEip;
    int         m_minDistanceToBottom;
    bool        m_isLeftDown;
    bool        m_showTargetNameInstead;
    int         m_jumpLineWidthMax;
    int         m_jumpLineWidthMin;
    int         m_jumpLineInstDistMax;

    wxBrush     m_curlineBrush;
    wxBrush     m_sellineBrush;
    wxBrush     m_zerolineBrush;
    wxColour    m_callRetColor;
    wxColour    m_jumpColor;

    Disassembler::InstDisasmMap     m_insts;
    std::map<u32, int>      m_eipIndex;
    std::vector<u32>        m_indexEip;
    std::map<int, int>      m_procEntryEnd;
    MutexCS *   m_mutex;
    //wxListCtrl  *   m_list;
    //Disassembler::InstVector    m_instVector;
};
 
#endif // __ARIETIS_GUI_CPUPANEL_H__