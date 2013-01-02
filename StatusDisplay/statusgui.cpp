#include "stdafx.h"
#include "StatusDisplay.h"
#include "statusgui.h"
#include "config.h"

enum {
    ID_Hello = 1,
    ID_Timer,
    ID_Text_UpdateInt,
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
    StatusDisplayFrame *frame = new StatusDisplayFrame( "LochsEmu Status", wxPoint(0, 0), wxSize(200, 150) );
    frame->Show( true );
    SetTopWindow(frame);
    return true;
}

StatusDisplayFrame::StatusDisplayFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size, wxCAPTION | wxCLIP_CHILDREN | wxCLOSE_BOX), 
    m_timer(this, ID_Timer)
{
    m_updateInterval = max(g_config.GetInt("General", "UpdateInterval", 1000), 50);
    InitUI();

    Bind(wxEVT_TIMER, &StatusDisplayFrame::OnTimer, this, ID_Timer);
    m_timer.Start(m_updateInterval);
    UpdateControls();
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
    wxMessageBox("Hello world from wxWidgets!", "haha", wxOK | wxICON_INFORMATION);
}

void StatusDisplayFrame::InitUI()
{
    SetBackgroundColour(wxColor("White"));
    //InitMenus();
    InitControls();
}

void StatusDisplayFrame::InitMenus()
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
    //CreateStatusBar();
    //SetStatusText( "Welcome to wxWidgets!" );

    Bind(wxEVT_COMMAND_MENU_SELECTED, &StatusDisplayFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &StatusDisplayFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &StatusDisplayFrame::OnAbout, this, wxID_ABOUT);
}

void StatusDisplayFrame::InitControls()
{
    wxPanel *panel = new wxPanel(this, -1);

    const int Border = 2;

    wxBoxSizer *sizer0 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *box0 = new wxStaticText(panel, -1, "Update Interval:");
    m_textInterval = new wxTextCtrl(panel, ID_Text_UpdateInt, wxString::Format("%d", m_updateInterval), 
        wxDefaultPosition, wxSize(40, 25));
    m_buttonApply = new wxButton(panel, wxID_APPLY, "&Apply", wxDefaultPosition, wxSize(40, 25));
    sizer0->Add(box0, 0, wxLEFT | wxTOP, Border);
    sizer0->Add(m_textInterval, 0, wxLEFT | wxTOP, Border);
    sizer0->Add(m_buttonApply, 0, wxLEFT | wxTOP, Border);
    m_buttonApply->Enable(false);

    Bind(wxEVT_COMMAND_TEXT_UPDATED, &StatusDisplayFrame::OnTextChange, this, ID_Text_UpdateInt);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &StatusDisplayFrame::OnApply, this, wxID_APPLY);

    wxBoxSizer *sizer1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *box1 = new wxStaticText(panel, -1, "CPU:", wxDefaultPosition, wxSize(50, -1));
    m_textCpuUsage = new wxStaticText(panel, -1, "");
    sizer1->Add(box1, 0, wxLEFT | wxTOP, Border);
    sizer1->Add(m_textCpuUsage, 0, wxLEFT | wxTOP | wxALIGN_LEFT, Border);

    wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *box2 = new wxStaticText(panel, -1, "MEM:", wxDefaultPosition, wxSize(50, -1));
    m_textMemUsage = new wxStaticText(panel, -1, "");
    sizer2->Add(box2, 0, wxLEFT | wxTOP, Border);
    sizer2->Add(m_textMemUsage, 0, wxLEFT | wxTOP | wxALIGN_LEFT, Border);

    wxBoxSizer *sizer3 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *box3 = new wxStaticText(panel, -1, "Threads:", wxDefaultPosition, wxSize(50, -1));
    m_textThreads = new wxStaticText(panel, -1, "");
    sizer3->Add(box3, 0, wxLEFT | wxTOP, Border);
    sizer3->Add(m_textThreads, 0, wxLEFT | wxTOP | wxALIGN_LEFT, Border);

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(sizer0);
    sizer->Add(sizer1);
    sizer->Add(sizer2);
    sizer->Add(sizer3);

    panel->SetSizer(sizer);
}

void StatusDisplayFrame::OnTimer( wxTimerEvent& event )
{
    UpdateControls();
}

void StatusDisplayFrame::UpdateControls()
{
    m_textCpuUsage->SetLabel(wxString::Format("%d %%", GetCpuUsage()));
    m_textMemUsage->SetLabel(wxString::Format("%d MB", GetMemUsage()));
    m_textThreads->SetLabel(wxString::Format("%d", GetThreadCount()));
}

void StatusDisplayFrame::OnTextChange( wxCommandEvent& event )
{
    wxString interval = m_textInterval->GetValue();
    long val;
    if (interval.ToLong(&val)) {
        m_textInterval->SetBackgroundColour(wxColour("White"));
        m_buttonApply->Enable(true);
    } else {
        m_textInterval->SetBackgroundColour(wxColour("#ff0000"));
        m_buttonApply->Enable(false);
    }
    m_textInterval->Refresh(true);
}

void StatusDisplayFrame::OnApply( wxCommandEvent& event )
{
    int interval = wxAtoi(m_textInterval->GetValue());
    g_config.SetInt("General", "UpdateInterval", interval);
    m_buttonApply->Enable(false);
    m_timer.Start(interval);
}

