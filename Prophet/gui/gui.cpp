#include "stdafx.h"
#include "gui.h"
#include "mainframe.h"
#include "Prophet.h"
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
    ProphetFrame *frame = new ProphetFrame(&g_engine, g_emulator);
    frame->Show(true);
    //SetTopWindow(frame);
    return true;
}

wxBrush     TaintBrushes[TaintBrushCount] = {
    wxBrush(wxColour("#0000ff")),
    wxBrush(wxColour("#0001ff")),
    wxBrush(wxColour("#0002ff")),
    wxBrush(wxColour("#0003ff")),
    wxBrush(wxColour("#0004ff")),
    wxBrush(wxColour("#0005ff")),
    wxBrush(wxColour("#0006ff")),
    wxBrush(wxColour("#0007ff")),
    wxBrush(wxColour("#0008ff")),
    wxBrush(wxColour("#0009ff")),
    wxBrush(wxColour("#000aff")),
    wxBrush(wxColour("#000bff")),
    wxBrush(wxColour("#000cff")),
    wxBrush(wxColour("#000dff")),
    wxBrush(wxColour("#000eff")),
    wxBrush(wxColour("#000fff")),
    wxBrush(wxColour("#0010ff")),
    wxBrush(wxColour("#0011ff")),
    wxBrush(wxColour("#0012ff")),
    wxBrush(wxColour("#0013ff")),
    wxBrush(wxColour("#0014ff")),
    wxBrush(wxColour("#0015ff")),
    wxBrush(wxColour("#0016ff")),
    wxBrush(wxColour("#0017ff")),
    wxBrush(wxColour("#0018ff")),
    wxBrush(wxColour("#0019ff")),
    wxBrush(wxColour("#001aff")),
    wxBrush(wxColour("#001bff")),
    wxBrush(wxColour("#001cff")),
    wxBrush(wxColour("#001dff")),
    wxBrush(wxColour("#001eff")),
    wxBrush(wxColour("#001fff")),
    wxBrush(wxColour("#0020ff")),
    wxBrush(wxColour("#0021ff")),
    wxBrush(wxColour("#0022ff")),
    wxBrush(wxColour("#0023ff")),
    wxBrush(wxColour("#0024ff")),
    wxBrush(wxColour("#0025ff")),
    wxBrush(wxColour("#0026ff")),
    wxBrush(wxColour("#0027ff")),
    wxBrush(wxColour("#0028ff")),
    wxBrush(wxColour("#0029ff")),
    wxBrush(wxColour("#002aff")),
    wxBrush(wxColour("#002bff")),
    wxBrush(wxColour("#002cff")),
    wxBrush(wxColour("#002dff")),
    wxBrush(wxColour("#002eff")),
    wxBrush(wxColour("#002fff")),
    wxBrush(wxColour("#0030ff")),
    wxBrush(wxColour("#0031ff")),
    wxBrush(wxColour("#0032ff")),
    wxBrush(wxColour("#0033ff")),
    wxBrush(wxColour("#0034ff")),
    wxBrush(wxColour("#0035ff")),
    wxBrush(wxColour("#0036ff")),
    wxBrush(wxColour("#0037ff")),
    wxBrush(wxColour("#0038ff")),
    wxBrush(wxColour("#0039ff")),
    wxBrush(wxColour("#003aff")),
    wxBrush(wxColour("#003bff")),
    wxBrush(wxColour("#003cff")),
    wxBrush(wxColour("#003dff")),
    wxBrush(wxColour("#003eff")),
    wxBrush(wxColour("#003fff")),
    wxBrush(wxColour("#0040ff")),
    wxBrush(wxColour("#0041ff")),
    wxBrush(wxColour("#0042ff")),
    wxBrush(wxColour("#0043ff")),
    wxBrush(wxColour("#0044ff")),
    wxBrush(wxColour("#0045ff")),
    wxBrush(wxColour("#0046ff")),
    wxBrush(wxColour("#0047ff")),
    wxBrush(wxColour("#0048ff")),
    wxBrush(wxColour("#0049ff")),
    wxBrush(wxColour("#004aff")),
    wxBrush(wxColour("#004bff")),
    wxBrush(wxColour("#004cff")),
    wxBrush(wxColour("#004dff")),
    wxBrush(wxColour("#004eff")),
    wxBrush(wxColour("#004fff")),
    wxBrush(wxColour("#0050ff")),
    wxBrush(wxColour("#0051ff")),
    wxBrush(wxColour("#0052ff")),
    wxBrush(wxColour("#0053ff")),
    wxBrush(wxColour("#0054ff")),
    wxBrush(wxColour("#0055ff")),
    wxBrush(wxColour("#0056ff")),
    wxBrush(wxColour("#0057ff")),
    wxBrush(wxColour("#0058ff")),
    wxBrush(wxColour("#0059ff")),
    wxBrush(wxColour("#005aff")),
    wxBrush(wxColour("#005bff")),
    wxBrush(wxColour("#005cff")),
    wxBrush(wxColour("#005dff")),
    wxBrush(wxColour("#005eff")),
    wxBrush(wxColour("#005fff")),
    wxBrush(wxColour("#0060ff")),
    wxBrush(wxColour("#0061ff")),
    wxBrush(wxColour("#0062ff")),
    wxBrush(wxColour("#0063ff")),
    wxBrush(wxColour("#0064ff")),
    wxBrush(wxColour("#0065ff")),
    wxBrush(wxColour("#0066ff")),
    wxBrush(wxColour("#0067ff")),
    wxBrush(wxColour("#0068ff")),
    wxBrush(wxColour("#0069ff")),
    wxBrush(wxColour("#006aff")),
    wxBrush(wxColour("#006bff")),
    wxBrush(wxColour("#006cff")),
    wxBrush(wxColour("#006dff")),
    wxBrush(wxColour("#006eff")),
    wxBrush(wxColour("#006fff")),
    wxBrush(wxColour("#0070ff")),
    wxBrush(wxColour("#0071ff")),
    wxBrush(wxColour("#0072ff")),
    wxBrush(wxColour("#0073ff")),
    wxBrush(wxColour("#0074ff")),
    wxBrush(wxColour("#0075ff")),
    wxBrush(wxColour("#0076ff")),
    wxBrush(wxColour("#0077ff")),
    wxBrush(wxColour("#0078ff")),
    wxBrush(wxColour("#0079ff")),
    wxBrush(wxColour("#007aff")),
    wxBrush(wxColour("#007bff")),
    wxBrush(wxColour("#007cff")),
    wxBrush(wxColour("#007dff")),
    wxBrush(wxColour("#007eff")),
    wxBrush(wxColour("#007fff")),
    wxBrush(wxColour("#0080ff")),
    wxBrush(wxColour("#0081ff")),
    wxBrush(wxColour("#0082ff")),
    wxBrush(wxColour("#0083ff")),
    wxBrush(wxColour("#0084ff")),
    wxBrush(wxColour("#0085ff")),
    wxBrush(wxColour("#0086ff")),
    wxBrush(wxColour("#0087ff")),
    wxBrush(wxColour("#0088ff")),
    wxBrush(wxColour("#0089ff")),
    wxBrush(wxColour("#008aff")),
    wxBrush(wxColour("#008bff")),
    wxBrush(wxColour("#008cff")),
    wxBrush(wxColour("#008dff")),
    wxBrush(wxColour("#008eff")),
    wxBrush(wxColour("#008fff")),
    wxBrush(wxColour("#0090ff")),
    wxBrush(wxColour("#0091ff")),
    wxBrush(wxColour("#0092ff")),
    wxBrush(wxColour("#0093ff")),
    wxBrush(wxColour("#0094ff")),
    wxBrush(wxColour("#0095ff")),
    wxBrush(wxColour("#0096ff")),
    wxBrush(wxColour("#0097ff")),
    wxBrush(wxColour("#0098ff")),
    wxBrush(wxColour("#0099ff")),
    wxBrush(wxColour("#009aff")),
    wxBrush(wxColour("#009bff")),
    wxBrush(wxColour("#009cff")),
    wxBrush(wxColour("#009dff")),
    wxBrush(wxColour("#009eff")),
    wxBrush(wxColour("#009fff")),
    wxBrush(wxColour("#00a0ff")),
    wxBrush(wxColour("#00a1ff")),
    wxBrush(wxColour("#00a2ff")),
    wxBrush(wxColour("#00a3ff")),
    wxBrush(wxColour("#00a4ff")),
    wxBrush(wxColour("#00a5ff")),
    wxBrush(wxColour("#00a6ff")),
    wxBrush(wxColour("#00a7ff")),
    wxBrush(wxColour("#00a8ff")),
    wxBrush(wxColour("#00a9ff")),
    wxBrush(wxColour("#00aaff")),
    wxBrush(wxColour("#00abff")),
    wxBrush(wxColour("#00acff")),
    wxBrush(wxColour("#00adff")),
    wxBrush(wxColour("#00aeff")),
    wxBrush(wxColour("#00afff")),
    wxBrush(wxColour("#00b0ff")),
    wxBrush(wxColour("#00b1ff")),
    wxBrush(wxColour("#00b2ff")),
    wxBrush(wxColour("#00b3ff")),
    wxBrush(wxColour("#00b4ff")),
    wxBrush(wxColour("#00b5ff")),
    wxBrush(wxColour("#00b6ff")),
    wxBrush(wxColour("#00b7ff")),
    wxBrush(wxColour("#00b8ff")),
    wxBrush(wxColour("#00b9ff")),
    wxBrush(wxColour("#00baff")),
    wxBrush(wxColour("#00bbff")),
    wxBrush(wxColour("#00bcff")),
    wxBrush(wxColour("#00bdff")),
    wxBrush(wxColour("#00beff")),
    wxBrush(wxColour("#00bfff")),
    wxBrush(wxColour("#00c0ff")),
    wxBrush(wxColour("#00c1ff")),
    wxBrush(wxColour("#00c2ff")),
    wxBrush(wxColour("#00c3ff")),
    wxBrush(wxColour("#00c4ff")),
    wxBrush(wxColour("#00c5ff")),
    wxBrush(wxColour("#00c6ff")),
    wxBrush(wxColour("#00c7ff")),
    wxBrush(wxColour("#00c8ff")),
    wxBrush(wxColour("#00c9ff")),
    wxBrush(wxColour("#00caff")),
    wxBrush(wxColour("#00cbff")),
    wxBrush(wxColour("#00ccff")),
    wxBrush(wxColour("#00cdff")),
    wxBrush(wxColour("#00ceff")),
    wxBrush(wxColour("#00cfff")),
    wxBrush(wxColour("#00d0ff")),
    wxBrush(wxColour("#00d1ff")),
    wxBrush(wxColour("#00d2ff")),
    wxBrush(wxColour("#00d3ff")),
    wxBrush(wxColour("#00d4ff")),
    wxBrush(wxColour("#00d5ff")),
    wxBrush(wxColour("#00d6ff")),
    wxBrush(wxColour("#00d7ff")),
    wxBrush(wxColour("#00d8ff")),
    wxBrush(wxColour("#00d9ff")),
    wxBrush(wxColour("#00daff")),
    wxBrush(wxColour("#00dbff")),
    wxBrush(wxColour("#00dcff")),
    wxBrush(wxColour("#00ddff")),
    wxBrush(wxColour("#00deff")),
    wxBrush(wxColour("#00dfff")),
    wxBrush(wxColour("#00e0ff")),
    wxBrush(wxColour("#00e1ff")),
    wxBrush(wxColour("#00e2ff")),
    wxBrush(wxColour("#00e3ff")),
    wxBrush(wxColour("#00e4ff")),
    wxBrush(wxColour("#00e5ff")),
    wxBrush(wxColour("#00e6ff")),
    wxBrush(wxColour("#00e7ff")),
    wxBrush(wxColour("#00e8ff")),
    wxBrush(wxColour("#00e9ff")),
    wxBrush(wxColour("#00eaff")),
    wxBrush(wxColour("#00ebff")),
    wxBrush(wxColour("#00ecff")),
    wxBrush(wxColour("#00edff")),
    wxBrush(wxColour("#00eeff")),
    wxBrush(wxColour("#00efff")),
    wxBrush(wxColour("#00f0ff")),
    wxBrush(wxColour("#00f1ff")),
    wxBrush(wxColour("#00f2ff")),
    wxBrush(wxColour("#00f3ff")),
    wxBrush(wxColour("#00f4ff")),
    wxBrush(wxColour("#00f5ff")),
    wxBrush(wxColour("#00f6ff")),
    wxBrush(wxColour("#00f7ff")),
    wxBrush(wxColour("#00f8ff")),
    wxBrush(wxColour("#00f9ff")),
    wxBrush(wxColour("#00faff")),
    wxBrush(wxColour("#00fbff")),
    wxBrush(wxColour("#00fcff")),
    wxBrush(wxColour("#00fdff")),
    wxBrush(wxColour("#00feff")),
    wxBrush(wxColour("#00ffff")),
};

void DrawTaint( wxBufferedPaintDC &dc, const Taint &t, const wxRect &rect, bool highlight )
{
    wxRect rectTaint(rect.x+1, rect.y, rect.width, rect.height-1);

    if (t.IsAllTainted()) {
        dc.SetBrush(*wxBLUE_BRUSH);
        dc.DrawRectangle(rectTaint);
    } else if (t.IsAllUntainted()) {
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRectangle(rectTaint);
    } else {
        const int TaintWidth = t.GetWidth();
        const float w = rectTaint.width / (float) TaintWidth;
        bool currentTaint = t.IsTainted(0);
        int sameTaintLen = 0;
        int sameTaintStart = 0;
        for (int i = 0; i < TaintWidth; i++) {
            if (t.IsTainted(i) == currentTaint) {
                sameTaintLen++;
            } 
            if (i < TaintWidth - 1) continue;
            int xOffset = rectTaint.width * i / (float) TaintWidth;
            dc.SetBrush(currentTaint ? *wxBLUE_BRUSH : *wxWHITE_BRUSH);
            int currTaintWidth = static_cast<int>(w * sameTaintLen);
            if (currentTaint)
                currTaintWidth = min(currTaintWidth, 1);
            dc.DrawRectangle(rectTaint.x + xOffset, rectTaint.y, currTaintWidth, rectTaint.height);

            currentTaint = !currentTaint;
            sameTaintLen = 0;
            sameTaintStart = i;
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
