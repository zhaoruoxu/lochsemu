#include "stdafx.h"
#include "mainframe.h"

#include "logpanel.h"
#include "cpupanel.h"
#include "tracepanel.h"
#include "contextpanel.h"

#include "utilities.h"
#include "engine.h"

#include "instruction.h"
#include "processor.h"

ArietisFrame::ArietisFrame(AEngine *engine)
    : m_engine(engine), wxFrame(NULL, wxID_ANY, "Arietis", wxDefaultPosition, wxSize(1000, 800), 
    wxDEFAULT_FRAME_STYLE), m_statusTimer(this, ID_StatusTimer)
{
    InitMisc();
    InitUI();

    m_engine->SetGuiFrame(this);
    NotifyMainThread();
}

ArietisFrame::~ArietisFrame()
{
    m_auiManager.UnInit();
}

void ArietisFrame::InitMisc()
{
    std::string cfgFile = LxGetModuleDirectory(g_module) + "arietisapp.ini";
    g_config.Initialize(cfgFile.c_str());
}

void ArietisFrame::InitUI()
{
    InitMenu();
    InitStatusBar();
    //InitToolbars();

    Bind(wxEVT_CLOSE_WINDOW, &ArietisFrame::OnClose, this, wxID_ANY);

    m_auiManager.SetManagedWindow(this);

    m_cpuPanel      = new CpuPanel(this);
    m_contextPanel  = new ContextPanel(this);
    m_tracePanel    = new CompositeTracePanel(this);

    
    m_auiManager.AddPane(m_cpuPanel, wxAuiPaneInfo().Name("Taint").Caption("CPU").Center());
    m_auiManager.AddPane(m_tracePanel, wxAuiPaneInfo().Name("Trace").Caption("Trace").Top());
    m_auiManager.AddPane(m_contextPanel, wxAuiPaneInfo().Name("Context").Caption("Context").Right());
    
    m_auiManager.Update();
    m_defaultPerspective = m_auiManager.SavePerspective();
    Centre();

    if (g_config.GetInt("General", "AutoLoadPerpective", 1) != 0) {
        OnLoadPerspective(wxCommandEvent());
    }
}

void ArietisFrame::InitMenu()
{
    wxMenu *menuFile = new wxMenu;
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "E&xit");

    wxMenu *menuView = new wxMenu;
    menuView->Append(ID_LoadPerspective, "Load perspective");
    menuView->Append(ID_SavePerspective, "Save perspective");
    menuView->Append(ID_ResetPerspective, "Reset perspective");

    wxMenu *menuDebug = new wxMenu;

    menuDebug->Append(ID_Run, "Run\tF5");
    menuDebug->Append(ID_StepOver, "Step Over\tF10");
    menuDebug->Append(ID_StepInto, "Step Into\tF11");
    menuDebug->Append(ID_StepOut, "Step Out\tShift-F10");
    menuDebug->AppendSeparator();
    menuDebug->Append(ID_ToggleBreakpoint, "Toggle Breakpoint\tF2");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABORT, "&About");

    

    wxMenuBar *menu = new wxMenuBar;
    menu->Append(menuFile, "&File");
    menu->Append(menuView, "&View");
    menu->Append(menuDebug, "&Debug");
    menu->Append(menuHelp, "&Help");
    SetMenuBar(menu);

    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnAbout, this, wxID_ABORT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnLoadPerspective, this, ID_LoadPerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnSavePerspective, this, ID_SavePerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnResetPerspective, this, ID_ResetPerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnStepInto, this, ID_StepInto);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnStepOver, this, ID_StepOver);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnStepOut, this, ID_StepOut);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnRun, this, ID_Run);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnToggleBreakpoint, this, ID_ToggleBreakpoint);
}



void ArietisFrame::InitStatusBar()
{
    static const int Widths[] = { -1, 40, 70, 80, 75 };
    m_statusbar = CreateStatusBar(_countof(Widths));
    m_statusbar->SetStatusWidths(_countof(Widths), Widths);
//     m_statusbar->SetStatusText("0", 0);
//     m_statusbar->SetStatusText("1", 1);
//     m_statusbar->SetStatusText("2", 2);

    m_statusTimer.Start(g_config.GetInt("General", "StatusUpdateInterval", 1000));
    Bind(wxEVT_TIMER, &ArietisFrame::OnStatusTimer, this, ID_StatusTimer);
}


void ArietisFrame::InitToolbars()
{
    wxToolBar *tbDebug = CreateToolBar(wxTB_DOCKABLE | wxTB_HORZ_TEXT | wxTB_NOICONS, ID_ToolbarDebug, "Debug");

    wxBitmap bitmap(16, 15);

    //tbDebug->AddTool(ID_StepInto, "StepInto", bitmap);
    tbDebug->AddControl(new wxButton(tbDebug, ID_StepInto, "Step Into", 
        wxDefaultPosition, wxSize(60, -1)));

    tbDebug->Realize();

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnStepInto, this, ID_StepInto);
}


//////////////////////////////////////////////////////////////////////////
// Event handlers
//////////////////////////////////////////////////////////////////////////
void ArietisFrame::OnExit( wxCommandEvent &event )
{
    Close();
}

void ArietisFrame::OnAbout( wxCommandEvent &event )
{
    wxMessageBox("Arietis GUI application", "Arietis", wxOK | wxICON_INFORMATION);
}

void ArietisFrame::OnClose( wxCloseEvent &event )
{
    if (event.CanVeto()) {
        if (wxMessageBox("Closing this window will terminate emulation.\n\nYou sure?", "Don't", wxICON_QUESTION | wxYES_NO) != wxYES) {
            event.Veto();
            return;
        }
    }
    if (g_config.GetInt("General", "AutoSavePerspective", 1) != 0) {
        OnSavePerspective(wxCommandEvent());
    }
    Destroy();
    m_engine->GetDebugger()->OnTerminate();
}

void ArietisFrame::OnSavePerspective( wxCommandEvent &event )
{
    wxString p = m_auiManager.SavePerspective();
    g_config.SetString("View", "Perspective", p.c_str().AsChar());
}

void ArietisFrame::OnLoadPerspective( wxCommandEvent &event )
{
    wxString p(g_config.GetString("View", "Perspective"));

    if (p.Length() > 0) {
        m_auiManager.LoadPerspective(p);
    }
}

void ArietisFrame::OnResetPerspective( wxCommandEvent &event )
{
    m_auiManager.LoadPerspective(m_defaultPerspective);
    g_config.SetString("View", "Perspective", m_defaultPerspective.c_str().AsChar());
}

void ArietisFrame::OnStatusTimer( wxTimerEvent &event )
{
    m_statusbar->SetStatusText(wxString::Format("CPU: %d%%", GetCpuUsage()), 2);
    m_statusbar->SetStatusText(wxString::Format("MEM: %d MB", GetMemUsage()), 3);
    m_statusbar->SetStatusText(wxString::Format("Threads: %d", GetThreadCount()), 4);
}

void ArietisFrame::OnStepInto( wxCommandEvent &event )
{
    m_engine->GetDebugger()->OnStepInto();
}

void ArietisFrame::OnRun( wxCommandEvent &event )
{
    m_engine->GetDebugger()->OnRun();
}

void ArietisFrame::OnStepOver( wxCommandEvent &event )
{
    m_engine->GetDebugger()->OnStepOver();
}

void ArietisFrame::OnStepOut( wxCommandEvent &event )
{
    m_engine->GetDebugger()->OnStepOut();
}

void ArietisFrame::OnToggleBreakpoint( wxCommandEvent &event )
{
    m_engine->GetDebugger()->OnToggleBreakpoint();
}

void ArietisFrame::PreExecSingleStepCallback( const Processor *cpu, const Instruction *inst )
{
    m_contextPanel->UpdateData(m_engine->GetCurrentInstContext(), "Dynamic Execution");
    m_cpuPanel->OnPtrChange(cpu->EIP);
    m_tracePanel->UpdateData();
}

void ArietisFrame::PostExecSingleStepCallback( const Processor *cpu, const Instruction *inst )
{
    //m_tracePanel->UpdateData();
}

void ArietisFrame::ReportBusy( bool isBusy )
{
    m_statusbar->SetStatusText(isBusy ? "Busy" : "", 1);
}

