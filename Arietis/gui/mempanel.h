#pragma once
 
#ifndef __ARIETIS_GUI_MEMPANEL_H__
#define __ARIETIS_GUI_MEMPANEL_H__
 
#include "Arietis.h"
#include "gui.h"
#include "custombase.h"
#include "core/memdesc.h"


struct SectionContext {
    u32         Base;
    u32         Size;
    wxString    ModuleName;
    wxString    Desc;
    u32         ModuleImageBase;

    SectionContext(const SectionInfo &sec, const ModuleInfo *minfo);
};

class MemInfoPanel : public SelectableScrolledControl {
public:
    MemInfoPanel(wxWindow *parent);
    ~MemInfoPanel();

    void        UpdateData(const Emulator *emu, const Memory *mem);
    void        OnLeftDoubleClick(wxMouseEvent &event);
private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawItem(wxBufferedPaintDC &dc, int index);
private:
    //const Memory *  m_memory;
    std::vector<SectionContext>     m_data;

    int         m_widthRange;
    int         m_widthSize;
    int         m_widthModule;
    int         m_widthDesc;
    int         m_width;

    wxBrush     m_currSelBrush;

    MutexCS *   m_mutex;
};

 
#endif // __ARIETIS_GUI_MEMPANEL_H__