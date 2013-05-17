#include "stdafx.h"
#include "gui.h"
#include "mainframe.h"
#include "prophet.h"
#include "parallel.h"
#include "taint/taint.h"

HANDLE      g_guiThread;

static DWORD __stdcall GuiThreadProc(LPVOID lpParams);

void RunGUI()
{
    g_guiThread = CreateThread(NULL, 0, GuiThreadProc, NULL, 0, NULL);
    if (INVALID_HANDLE_VALUE == g_guiThread) {
        LxError("Cannot create Prophet GUI thread\n");
    }
}

DWORD __stdcall GuiThreadProc( LPVOID lpParams )
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    wxEntry(hInstance);
    return 0;
}

ProphetApp& wxGetApp() { return *static_cast<ProphetApp*>(wxApp::GetInstance()); }

wxAppConsole *wxCreateApp()
{
    wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "ProphetApp");
    return new ProphetApp;
}

void NotifyMainThread()
{
    g_guiSem.Post();
}

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);

bool ProphetApp::OnInit()
{
    m_frame = new ProphetFrame(&g_engine, g_emulator);
    m_frame->Show(true);
    return true;
}

int ProphetApp::OnExit()
{
    //delete m_frame;       // fuck this
    return 0;
}

void DrawTaint( wxBufferedPaintDC &dc, const Taint &t, const wxRect &rect, bool highlight )
{
    wxRect rectTaint(rect.x, rect.y, rect.width, rect.height-1);

    if (t.IsAllTainted()) {
        dc.SetBrush(*wxBLUE_BRUSH);
        dc.DrawRectangle(rectTaint);
    } else if (t.IsAllUntainted()) {
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRectangle(rectTaint);
    } else {
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRectangle(rectTaint);

        dc.SetBrush(*wxBLUE_BRUSH);
        const int TaintWidth = t.GetWidth();
        const float w = rectTaint.width / (float) TaintWidth;
        
        bool currTainted = t.IsTainted(0);
        int taintedStart = 0;
        for (int i = 0; i < TaintWidth; i++) {
            if (t.IsTainted(i)) {
                if (!currTainted) {
                    currTainted = true;
                    taintedStart = i;
                }
            } 
            if (currTainted && (!t.IsTainted(i) || i == TaintWidth-1)) {
                int xOffset = static_cast<int>(w * taintedStart + 0.5f);
                int width = max(static_cast<int>(w * (i - taintedStart) + 0.5f), 2);
                dc.DrawRectangle(rectTaint.x + xOffset, rectTaint.y, width, rectTaint.height);
                currTainted = false;
            }
        }
    }

    if (highlight) {
        dc.SetPen(wxPen(wxColour("#ff4000")));
        dc.DrawLine(rect.x, rect.y, rect.x + rect.width, rect.y);
        dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, 
            rect.y + rect.height - 1);
        dc.SetPen(*wxTRANSPARENT_PEN);
    } else {
        dc.SetPen(wxPen(wxColour("#e0e0e0")));
        dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, 
            rect.y + rect.height - 1);
        dc.SetPen(*wxTRANSPARENT_PEN);
    }
}

bool GetU32FromUser( const wxString &msg, u32p val )
{
    wxString str = wxGetTextFromUser(msg);
    if (str.IsEmpty()) return false;
    if (!str.ToULong((unsigned long *)val, 16)) {
        wxMessageBox("Invalid hex value:" + str);
        return false;
    }
    return true;
}
