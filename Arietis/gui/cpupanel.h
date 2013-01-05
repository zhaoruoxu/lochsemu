#pragma once
 
#ifndef __ARIETIS_GUI_CPUPANEL_H__
#define __ARIETIS_GUI_CPUPANEL_H__
 
#include "Arietis.h"
#include "gui.h"
#include "../static/disassembler.h"

class CpuPanel : public wxScrolledWindow {
public:
    CpuPanel(wxWindow *parent);

//     void        OnInstDisasm(const Disassembler::InstVector &insts);
    void        OnPtrChange(u32 addr);
    void        OnDataUpdate(const Disassembler::InstDisasmMap *insts);
    void        OnPreExecute(const Processor *cpu, const Instruction *inst);


    void        OnPaint(wxPaintEvent& event);
    void        OnEraseBackground(wxEraseEvent& event);
    void        OnLeftClick(wxMouseEvent& event);
    //wxCoord     OnGetRowHeight(size_t row) const override;
    //wxCoord     OnGetColumnWidth(size_t column) const override;
private:
    void        InitLogic();
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc);
    void        DrawInst(wxBufferedPaintDC &dc, const Disassembler::Inst &inst, int index);
    wxPoint     GetCurrentScrolledPos() const;
private:
    
    /* render */
    int         m_widthIp;
    int         m_widthDisasm;
    int         m_widthInfo;
    int         m_width;
    int         m_height;
    int         m_lineHeight;
    wxFontMetrics   m_fontMetrics;
    int         m_currIndex;
    int         m_currSelIndex;
    int         m_minDistanceToBottom;
    wxBrush     m_curlineBrush;
    wxBrush     m_sellineBrush;

    wxFont      m_font;
    wxDataViewListCtrl *    m_list;
    const Disassembler::InstDisasmMap *   m_insts;
    std::map<u32, int>      m_eipIndex;
    //wxListCtrl  *   m_list;
    //Disassembler::InstVector    m_instVector;
};
 
#endif // __ARIETIS_GUI_CPUPANEL_H__