#include "stdafx.h"
#include "mainframe.h"

#include "logpanel.h"
#include "cpupanel.h"
#include "tracepanel.h"
#include "contextpanel.h"
#include "mempanel.h"
#include "bpspanel.h"

#include "utilities.h"
#include "engine.h"

#include "instruction.h"
#include "processor.h"

#include "buildver.h"

ArietisFrame::ArietisFrame(AEngine *engine)
    : m_engine(engine), wxFrame(NULL, wxID_ANY, 
    wxString::Format("Arietis %x build %d", ArietisVersion, ARIETIS_BUILD_VERSION), 
    wxDefaultPosition, wxSize(850, 850), wxDEFAULT_FRAME_STYLE), 
    m_statusTimer(this, ID_StatusTimer)
{
    InitMisc();
    InitUI();

    m_engine->SetGuiFrame(this);
    NotifyMainThread();
    m_archive = NULL;
}

ArietisFrame::~ArietisFrame()
{
    m_auiManager.UnInit();
}

void ArietisFrame::InitMisc()
{
    //std::string cfgFile = LxGetModuleDirectory(g_module) + "arietisapp.ini";
    //g_config.Initialize(cfgFile.c_str());
}

void ArietisFrame::InitUI()
{
    InitMenu();
    InitStatusBar();
    InitToolbars();

    Bind(wxEVT_CLOSE_WINDOW, &ArietisFrame::OnClose, this, wxID_ANY);

    m_auiManager.SetManagedWindow(this);

    m_cpuPanel      = new CpuPanel(this, m_engine);
    m_contextPanel  = new ContextPanel(this);
    m_tracePanel    = new CompositeTracePanel(this, m_contextPanel);
    m_memDataPanel  = new MemDataPanel(this);
    m_memInfoPanel  = new MemInfoPanel(this, m_memDataPanel);
    m_bpsPanel      = new BreakpointsPanel(this);
    
    
    m_auiManager.AddPane(m_cpuPanel, 
        wxAuiPaneInfo().Name("CPU").Caption("CPU").CenterPane());
    m_auiManager.AddPane(m_tracePanel, 
        wxAuiPaneInfo().Name("Trace").Caption("Trace").Top());
    m_auiManager.AddPane(m_contextPanel, 
        wxAuiPaneInfo().Name("Context").Caption("Context").Right());
    m_auiManager.AddPane(m_memInfoPanel, 
        wxAuiPaneInfo().Name("Sections").Caption("Sections").Bottom().Position(0).Row(1));
    m_auiManager.AddPane(m_memDataPanel, 
        wxAuiPaneInfo().Name("Memory").Caption("Memory").Bottom().Position(1).Row(1));
    m_auiManager.AddPane(m_bpsPanel, 
        wxAuiPaneInfo().Name("Breakpoints").Caption("Breakpoints").Bottom().Row(0));
    
    m_auiManager.Update();
    Centre();

    m_defaultPerspective = m_auiManager.SavePerspective();

    if (g_config.GetInt("General", "AutoLoadPerpective", 1) != 0) {
        OnLoadPerspective(wxCommandEvent());
    }

    
    //SetSize(800, 800);
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
    menuDebug->Append(ID_RemoveBreakpoint, "Remove Breakpoint\tF3");

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
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnRemoveBreakpoint, this, ID_RemoveBreakpoint);
}

enum StatusbarText {
    Statusbar_Path = 0,
    Statusbar_Tracing,
    Statusbar_Busy,
    Statusbar_Cpu,
    Statusbar_Mem,
    Statusbar_Threads,
};

void ArietisFrame::InitStatusBar()
{
    static const int Widths[] = { -1, 50, 40, 70, 100, 75 };
    m_statusbar = CreateStatusBar(_countof(Widths));
    m_statusbar->SetStatusWidths(_countof(Widths), Widths);

    m_statusTimer.Start(g_config.GetInt("General", "StatusUpdateInterval", 1000));
    Bind(wxEVT_TIMER, &ArietisFrame::OnStatusTimer, this, ID_StatusTimer);
}


void ArietisFrame::InitToolbars()
{
    wxToolBar *tbDebug = CreateToolBar(wxTB_DOCKABLE | wxTB_HORZ_TEXT | wxTB_NOICONS, ID_ToolbarDebug, "Debug");

    wxBitmap bitmap(16, 15);

    tbDebug->AddControl(new wxButton(tbDebug, ID_StepInto, "Step Into", wxDefaultPosition, 
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_StepOver, "Step Over", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_StepOut, "Step Out", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_Run, "Run", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddSeparator();
    m_toggleTraceButton = new wxButton(tbDebug, ID_ToolbarToggleTrace, "Invalid", wxDefaultPosition, 
        wxSize(80, -1), wxBORDER_NONE);
    tbDebug->AddControl(m_toggleTraceButton);

    tbDebug->Realize();

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnStepInto,   this, ID_StepInto);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnStepOver,   this, ID_StepOver);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnStepOut,    this, ID_StepOut);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnRun,        this, ID_Run);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnToggleTraceClicked, this, ID_ToolbarToggleTrace);
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
    m_engine->Terminate();
    //m_engine->GetDebugger()->OnTerminate();
}

void ArietisFrame::OnSavePerspective( wxCommandEvent &event )
{
    wxString p = m_auiManager.SavePerspective();
    g_config.SetString("View", "Perspective", p.c_str().AsChar());

    wxSize s = GetSize();
    wxPoint pt = GetPosition();

    g_config.SetInt("View", "Width", s.GetWidth());
    g_config.SetInt("View", "Height", s.GetHeight());
    g_config.SetInt("View", "PosX", pt.x);
    g_config.SetInt("View", "PosY", pt.y);
    g_config.SetInt("View", "Maximized", IsMaximized());
}

void ArietisFrame::OnLoadPerspective( wxCommandEvent &event )
{
    wxString p(g_config.GetString("View", "Perspective"));

    if (p.Length() > 0) {
        m_auiManager.LoadPerspective(p);
    }

    wxSize s(g_config.GetInt("View", "Width", 800), 
        g_config.GetInt("View", "Height", 800));
    wxPoint pt(g_config.GetInt("View", "PosX", 0),
        g_config.GetInt("View", "PosY", 0));
    bool maximized = g_config.GetInt("View", "Maximized", false) != 0;

    SetSize(s);
    SetPosition(pt);
    Maximize(maximized);
}

void ArietisFrame::OnResetPerspective( wxCommandEvent &event )
{
    m_auiManager.LoadPerspective(m_defaultPerspective);
    g_config.SetString("View", "Perspective", m_defaultPerspective.c_str().AsChar());

    wxSize defSize(800, 800);
    SetSize(defSize);
    Center();
    wxPoint defPos = GetPosition();
    Maximize(false);

    g_config.SetInt("View", "Width", defSize.GetWidth());
    g_config.SetInt("View", "Height", defSize.GetHeight());
    g_config.SetInt("View", "PosX", defPos.x);
    g_config.SetInt("View", "PosY", defPos.y);
    g_config.SetInt("View", "Maximized", false);
}

void ArietisFrame::OnStatusTimer( wxTimerEvent &event )
{
    m_statusbar->SetStatusText(wxString::Format("CPU: %d%%", GetCpuUsage()), Statusbar_Cpu);
    m_statusbar->SetStatusText(wxString::Format("MEM: %d MB", GetMemUsage()), Statusbar_Mem);
    m_statusbar->SetStatusText(wxString::Format("Threads: %d", GetThreadCount()), Statusbar_Threads);
}

void ArietisFrame::OnStepInto( wxCommandEvent &event )
{
    m_engine->GetDebugger()->OnStepInto();
}

void ArietisFrame::OnRun( wxCommandEvent &event )
{
    OnSavePerspective(wxCommandEvent());
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
    u32 eip = m_cpuPanel->GetSelectedEip();
    if (eip == 0) eip = m_cpuPanel->GetCurrentEip();
    Assert(eip != 0);
    m_engine->GetDebugger()->ToggleBreakpoint(eip);
    m_bpsPanel->UpdateData(m_engine);
    m_cpuPanel->Refresh();
}

void ArietisFrame::OnRemoveBreakpoint( wxCommandEvent &event )
{
    u32 eip = m_cpuPanel->GetSelectedEip();
    if (eip == 0) eip = m_cpuPanel->GetCurrentEip();
    Assert(eip != 0);
    m_engine->GetDebugger()->RemoveBreakpoint(eip);
    m_bpsPanel->UpdateData(m_engine);
    m_cpuPanel->Refresh();
}

void ArietisFrame::PreExecSingleStepCallback( const Processor *cpu, const Instruction *inst )
{
    InstContext ctx;
    m_engine->GetInstContext(&ctx);
    m_contextPanel->UpdateData(&ctx, "Dynamic Execution");
    m_cpuPanel->OnCurrentEipChange(cpu->EIP);
    m_tracePanel->UpdateData();
    m_memInfoPanel->UpdateData(cpu->Emu(), cpu->Mem);
    m_memDataPanel->Refresh();
    m_bpsPanel->UpdateData(m_engine);
}


void ArietisFrame::ReportBusy( bool isBusy )
{
    m_statusbar->SetStatusText(isBusy ? "Busy" : "", Statusbar_Busy);
}

void ArietisFrame::OnProcessLoaded( LPCSTR path )
{
    m_statusbar->SetStatusText(path, Statusbar_Path);
}

void ArietisFrame::OnToggleTrace( bool traceEnabled )
{
    m_engine->GetTracer()->Enable(traceEnabled);
    m_statusbar->SetStatusText(traceEnabled ? "Tracing" : "", Statusbar_Tracing);
    m_toggleTraceButton->SetLabel(traceEnabled ? "Stop Trace" : "Start Trace");
    m_toggleTraceButton->SetBackgroundColour(traceEnabled ? *wxRED : *wxCYAN);
}

void ArietisFrame::OnToggleTraceClicked( wxCommandEvent &event )
{
    OnToggleTrace(!m_engine->GetTracer()->IsEnabled());
    m_engine->SaveArchive();
}

void ArietisFrame::OnArchiveLoaded( Archive *arc )
{
    m_archive = arc;
    OnToggleTrace(m_engine->GetTracer()->IsEnabled());
}

