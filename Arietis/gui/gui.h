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

    ID_StatusTimer,

    /* toolbar */
    ID_ToolbarDebug,

    /* cpu panel */
    ID_CpuInstList,
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

#endif // __ARIETIS_GUI_GUI_H__