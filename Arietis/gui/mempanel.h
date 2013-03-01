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
    MemDataPanel(wxWindow *parent, AEngine *engine);
    ~MemDataPanel();
    
    void        Draw(wxBufferedPaintDC &dc) override;
    void        OnLeftDown(wxMouseEvent &event);
    void        OnLeftUp(wxMouseEvent &event);
    void        OnRightClick(wxMouseEvent &event);
    void        OnMouseMove(wxMouseEvent &event);
    void        OnMouseLeave(wxMouseEvent &event);
    void        OnTaintMemoryRangedClicked(wxCommandEvent &event);
    void        OnTaintMemoryRangedAllBitsClicked(wxCommandEvent &event);

    void        UpdateData(const Section *sec, const SectionContext &ctx);
    void        SelectAddress(u32 addr, u32 len);

private:
    void        InitRender();
    void        InitMenu();
    void        DrawLine(wxBufferedPaintDC &dc, int idx);
    int         GetIndex(const wxPoint &mouse);
    void        TaintMemRanged(bool allbits);
private:
    static const int    CharsPerLine = 8;
    const Section *     m_section;
    pbyte               m_rawdata;
    SectionContext      m_context;
    AEngine *           m_engine;
    TaintEngine *       m_taint;
    wxBrush             m_bgBrush;
    wxMenu *            m_popup;

    int         m_widthOffset;
    int         m_widthHex;
    int         m_widthChar;
    int         m_widthTaint;
    int         m_width;
    int         m_totalLines;

    bool        m_isLeftDown;
    int         m_selDown;
    int         m_selUp;

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