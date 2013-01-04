#include "stdafx.h"
#include "gui.h"
#include "mainframe.h"


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

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);

bool ArietisApp::OnInit()
{
    ArietisFrame *frame = new ArietisFrame;
    frame->Show(true);
    //SetTopWindow(frame);
    return true;
}
