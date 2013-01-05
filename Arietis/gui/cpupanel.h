#pragma once
 
#ifndef __ARIETIS_GUI_CPUPANEL_H__
#define __ARIETIS_GUI_CPUPANEL_H__
 
#include "Arietis.h"
#include "gui.h"
#include "../static/disassembler.h"

class CpuPanel : public wxPanel {
public:
    CpuPanel(wxWindow *parent);

//     void        OnInstDisasm(const Disassembler::InstVector &insts);
//     void        OnPtrChange(u32 addr);
    void        OnPreExecute(const Processor *cpu, const Instruction *inst);
private:
    void        InitUI();
    void        InitLogic();
private:
    wxFont      m_font;
    wxDataViewListCtrl *    m_list;
    //wxListCtrl  *   m_list;
    //Disassembler::InstVector    m_instVector;
};
 
#endif // __ARIETIS_GUI_CPUPANEL_H__