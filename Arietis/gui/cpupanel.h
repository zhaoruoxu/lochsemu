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
//     void        OnPtrChange(u32 addr);
    void        OnDataUpdate(const Disassembler::InstDisasmMap *insts);
    void        OnPreExecute(const Processor *cpu, const Instruction *inst);


    void        OnPaint(wxPaintEvent& event);
    void        OnEraseBackground(wxEraseEvent& event);
    //wxCoord     OnGetRowHeight(size_t row) const override;
    //wxCoord     OnGetColumnWidth(size_t column) const override;
private:
    void        InitUI();
    void        InitLogic();
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc);
    void        DrawInst(wxBufferedPaintDC &dc, const Disassembler::Inst &inst, int index);
private:
    
    /* render */
    int         m_widthIp;
    int         m_widthDisasm;
    int         m_width;
    int         m_height;
    wxFontMetrics   m_fontMetrics;

    wxFont      m_font;
    wxDataViewListCtrl *    m_list;
    const Disassembler::InstDisasmMap *   m_insts;
    //wxListCtrl  *   m_list;
    //Disassembler::InstVector    m_instVector;
};
 
#endif // __ARIETIS_GUI_CPUPANEL_H__