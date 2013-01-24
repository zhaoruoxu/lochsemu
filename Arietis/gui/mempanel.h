#pragma once
 
#ifndef __ARIETIS_GUI_MEMPANEL_H__
#define __ARIETIS_GUI_MEMPANEL_H__
 
#include "Arietis.h"
#include "gui.h"
#include "custombase.h"
#include "core/memdesc.h"

#include "parallel.h"

struct SectionContext {
    u32         Base;
    u32         Size;
    wxString    ModuleName;
    wxString    Desc;
    u32         ModuleImageBase;

    SectionContext();
    SectionContext(const SectionInfo &sec, const ModuleInfo *minfo);
};


class MemDataPanel : public CustomScrolledControl {
public:
    MemDataPanel(wxWindow *parent);
    ~MemDataPanel();
    
    void        Draw(wxBufferedPaintDC &dc) override;
    void        UpdateData(const Section *sec, const SectionContext &ctx);

private:
    void        InitRender();
    void        DrawLine(wxBufferedPaintDC &dc, int idx);
private:
    static const int    CharsPerLine = 16;
    const Section *     m_section;
    pbyte               m_rawdata;
    SectionContext      m_context;

    int         m_widthOffset;
    int         m_widthHex;
    int         m_widthChar;
    int         m_width;
    int         m_totalLines;

    MutexCS     m_mutex;
};


class MemInfoPanel : public SelectableScrolledControl {
public:
    MemInfoPanel(wxWindow *parent, MemDataPanel *data);
    ~MemInfoPanel();

    void        UpdateData(const Emulator *emu, const Memory *mem);
    void        OnLeftDoubleClick(wxMouseEvent &event);
private:
    void        InitRender();
    void        Draw(wxBufferedPaintDC &dc) override;
    void        DrawItem(wxBufferedPaintDC &dc, int index);
private:
    const Memory *  m_memory;
    std::vector<SectionContext>     m_data;
    MemDataPanel *  m_dataPanel;

    int         m_widthRange;
    int         m_widthSize;
    int         m_widthModule;
    int         m_widthDesc;
    int         m_width;

    wxBrush     m_currSelBrush;

    MutexCS     m_mutex;
};

 
#endif // __ARIETIS_GUI_MEMPANEL_H__