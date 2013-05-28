#pragma once
 
#ifndef __PROPHET_GUI_GUI_H__
#define __PROPHET_GUI_GUI_H__
 
#include "prophet.h"
#include "protocol/taint/taint.h"

void RunGUI();
void WaitForGUI();
void NotifyMainThread();

class CustomScrolledControl;
class ProphetFrame;
class CpuPanel;
class ContextPanel;
class CompositeTracePanel;
class MemInfoPanel;
class MemDataPanel;
class BreakpointsPanel;
class MessagePanel;
class MySwitch;
class StatPanel;
class StackPanel;
class ThreadPanel;

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
    ID_RunNoBp,
    ID_ToggleBreakpoint,
    ID_RemoveBreakpoint,
    ID_ShowMemory,
    ID_ShowCode,

    /* toolbar */
    ID_ToolbarDebug,
    ID_ToolbarToggleTrace,
    //ID_ToolbarToggleCRTEntry,
    //ID_ToolbarToggleSkipDllEntry,
    ID_ToolbarToggleTaint,

    /* notebooks */
    ID_NbContext,
    ID_NbSections,
    ID_NbTrace,
    ID_NbStack,

    /* cpu panel */
    ID_CpuInstList,
    ID_PopupShowCurrInst,
    ID_PopupShowTargetInst,
    ID_PopupTaintReg,
    ID_PopupTrackMemory,

    /* Bps panel */
    ID_PopupShowCode,
    ID_PopupDelete,
    ID_PopupToggle,

    /* mem data panel */
    ID_PopupTaintMemRange,
    ID_PopupTaintMemRangeAllBits,

    /* trace panel */
    ID_PopupFindFirstReg,
    ID_PopupFindMrAddr,
    ID_PopupFindMwAddr,

    ID_StatusTimer,
    ID_StatTimer,

    /* plugins menu */
    ID_PluginCheckEnable,
    // ID_PluginCheckEnable+1, +2, ...
};

class ProphetApp : public wxApp {
public:
    virtual bool        OnInit() override;
    virtual int         OnExit() override;

private:
    ProphetFrame *m_frame;

};

template <typename T>
bool    IntersectAbs(T i0, T i1, T istart, T iend) {
    return min(i0, i1) <= iend && max(i0, i1) >= istart;
}

template <typename T>
bool    Intersect(T ilow, T ihigh, T istart, T iend) {
    return ilow <= iend && ihigh >= istart;
}

template <typename T>
bool    InRange(T val, T start, T len) {
    return start <= val && val < start + len;
}

const int   TaintBrushCount = 256;
wxBrush     TaintBrushes[];

void DrawTaint(wxBufferedPaintDC &dc, const Taint &t, const wxRect &rect, bool highlight);

template <int N>
void DrawTaint(wxBufferedPaintDC &dc, const Tb<N> &t, const wxRect &rect, bool highlight = false)
{
    const int TaintWidth = t[0].GetWidth();
//     const int w = rect.width / TaintWidth;
//     if (w == 0) {
//         LxFatal("Drawing width for each taint value is be too small\n");
//     }
    dc.SetPen(*wxTRANSPARENT_PEN);
    wxRect r = rect;
    for (int i = 0; i < N; i++) {
        DrawTaint(dc, t[i], r, highlight);
        r.x += r.width+1;
    }
}

bool GetU32FromUser(const wxString &msg, u32p val);

#endif // __PROPHET_GUI_GUI_H__