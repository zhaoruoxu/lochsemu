#pragma once
 
#ifndef __ARIETIS_GUI_GUI_H__
#define __ARIETIS_GUI_GUI_H__
 
#include "Arietis.h"

void RunGUI();
void NotifyMainThread();

class CustomScrolledControl;
class ArietisFrame;
class CpuPanel;
class ContextPanel;
class CompositeTracePanel;
class MemInfoPanel;
class MemDataPanel;
class BreakpointsPanel;
class MySwitch;

enum {
    /* view */
    ID_LoadPerspective = 1,
    ID_SavePerspective,
    ID_ResetPerspective,

    /* debug */
    ID_StepInto,
    ID_StepOver,
    ID_StepOut,
    ID_Run,
    ID_ToggleBreakpoint,
    ID_RemoveBreakpoint,

    ID_StatusTimer,

    /* toolbar */
    ID_ToolbarDebug,
    ID_ToolbarToggleTrace,
    ID_ToolbarToggleCRTEntry,
    ID_ToolbarToggleSkipDllEntry,
    ID_ToolbarToggleTaint,

    /* cpu panel */
    ID_CpuInstList,
    ID_PopupShowCurrInst,
    ID_PopupTaintReg,

    /* Bps panel */
    ID_PopupShowCode,
    ID_PopupDelete,
    ID_PopupToggle,
};

class ArietisApp : public wxApp {
public:
    virtual bool        OnInit();
};

template <typename T>
bool    IntersectAbs(T i0, T i1, T istart, T iend) {
    return min(i0, i1) <= iend && max(i0, i1) >= istart;
}

template <typename T>
bool    Intersect(T ilow, T ihigh, T istart, T iend) {
    return ilow <= iend && ihigh >= istart;
}

void DrawTaint(wxBufferedPaintDC &dc, const Taint &t, const wxRect &rect);

#endif // __ARIETIS_GUI_GUI_H__