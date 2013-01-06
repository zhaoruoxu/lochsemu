#pragma once
 
#ifndef __ARIETIS_GUI_GUI_H__
#define __ARIETIS_GUI_GUI_H__
 
#include "Arietis.h"

void RunGUI();
void NotifyMainThread();

class ArietisFrame;
class LogPanel;
class CpuPanel;
class RegPanel;

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

#endif // __ARIETIS_GUI_GUI_H__