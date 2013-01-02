#include "stdafx.h"
#include "StatusDisplay.h"
#include "statusgui.h"

enum {
    ID_Hello = 1
};

// wxBEGIN_EVENT_TABLE(StatusDisplayFrame, wxFrame)
// EVT_MENU(ID_Hello, StatusDisplayFrame::OnHello)
// EVT_MENU(wxID_EXIT, StatusDisplayFrame::OnExit)
// EVT_MENU(wxID_ABOUT, StatusDisplayFrame::OnAbout)
// wxEND_EVENT_TABLE()

void RunGUI()
{
    //wxDISABLE_DEBUG_SUPPORT();

    HINSTANCE hInstance = GetModuleHandle(NULL);

    wxEntry(hInstance);
}

StatusDisplay& wxGetApp() { return *static_cast<StatusDisplay*>(wxApp::GetInstance()); }

wxAppConsole *wxCreateApp()
{
    wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "your program");
    return new StatusDisplay;
}

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);

bool StatusDisplay::OnInit()
{
    StatusDisplayFrame *frame = new StatusDisplayFrame( "Hello World", wxPoint(50, 50), wxSize(450, 340) );
    frame->Show( true );
    SetTopWindow(frame);
    return true;
}

StatusDisplayFrame::StatusDisplayFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
        "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );
    SetMenuBar( menuBar );
    wxString str("hahaha");
    CreateStatusBar();
    SetStatusText( "Welcome to wxWidgets!" );

//     Connect(ID_Hello, wxEVT_COMMAND_MENU_SELECTED, 
//         wxCommandEventHandler(StatusDisplayFrame::OnHello));
//     Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
//         wxCommandEventHandler(StatusDisplayFrame::OnExit));
//     Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
//         wxCommandEventHandler(StatusDisplayFrame::OnAbout));
    Bind(wxEVT_COMMAND_MENU_SELECTED, &StatusDisplayFrame::OnHello, this, ID_Hello);
}

void StatusDisplayFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}

void StatusDisplayFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( "This is a wxWidgets' Hello world sample",
        "About Hello World", wxOK | wxICON_INFORMATION );
}

void StatusDisplayFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

