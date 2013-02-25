#include "stdafx.h"
#include "gui.h"
#include "mainframe.h"
#include "Arietis.h"
#include "parallel.h"
#include "taint/taint.h"

HANDLE      g_guiThread;

static DWORD __stdcall GuiThreadProc(LPVOID lpParams);

void RunGUI()
{
    g_guiThread = CreateThread(NULL, 0, GuiThreadProc, NULL, 0, NULL);
    if (INVALID_HANDLE_VALUE == g_guiThread) {
        LxError("Cannot create Arietis GUI thread\n");
    }
}


DWORD __stdcall GuiThreadProc( LPVOID lpParams )
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    wxEntry(hInstance);
    return 0;
}

ArietisApp& wxGetApp() { return *static_cast<ArietisApp*>(wxApp::GetInstance()); }

wxAppConsole *wxCreateApp()
{
    wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "ArietisApp");
    return new ArietisApp;
}

void NotifyMainThread()
{
    g_guiSem.Post();
}

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);

bool ArietisApp::OnInit()
{
    ArietisFrame *frame = new ArietisFrame(&g_engine);
    frame->Show(true);
    //SetTopWindow(frame);
    return true;
}


void DrawTaint( wxBufferedPaintDC &dc, const Taint &t, const wxRect &rect )
{
    //dc.SetBrush(*wxWHITE_BRUSH);
    //dc.DrawRectangle(rect);
    const int TaintWidth = t.GetWidth();
    int w = rect.width / TaintWidth;
    if (w <= 2) {
        LxWarning("Drawing width for each taint value might be too small\n");
    }
    dc.SetPen(*wxTRANSPARENT_PEN);
    for (int i = 0; i < TaintWidth; i++) {
        int xOffset = rect.width * i / TaintWidth;
        dc.SetBrush(t.IsTainted(i) ? *wxRED_BRUSH : *wxWHITE_BRUSH);
        dc.DrawRectangle(rect.x + xOffset, rect.y, w - 1, rect.height - 1);
    }
}