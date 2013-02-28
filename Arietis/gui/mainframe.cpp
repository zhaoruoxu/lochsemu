#include "stdafx.h"
#include "mainframe.h"

#include "logpanel.h"
#include "cpupanel.h"
#include "tracepanel.h"
#include "contextpanel.h"
#include "mempanel.h"
#include "bpspanel.h"
#include "myswitch.h"

#include "utilities.h"
#include "engine.h"

#include "instruction.h"
#include "processor.h"

#include "buildver.h"

ArietisFrame::ArietisFrame(AEngine *engine, Emulator *emu)
    : m_engine(engine), m_emulator(emu), wxFrame(NULL, wxID_ANY, 
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
    m_memDataPanel  = new MemDataPanel(this, m_engine);
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
        wxAuiPaneInfo().Name("Memory").Caption("Memory").Bottom().Row(0));
    m_auiManager.AddPane(m_bpsPanel, 
        wxAuiPaneInfo().Name("Breakpoints").Caption("Breakpoints").Bottom().Position(1).Row(1));
    
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
    menuFile->Append(wxID_EXIT, "Exit\tCtrl-Q");

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
    menuDebug->AppendSeparator();
    menuDebug->Append(ID_ShowMemory, "Show memory by address\tCtrl-M");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABORT, "&About");

    

    wxMenuBar *menu = new wxMenuBar;
    menu->Append(menuFile, "&File");
    menu->Append(menuView, "&View");
    menu->Append(menuDebug, "&Debug");
    menu->Append(menuHelp, "&Help");
    SetMenuBar(menu);

    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnExit,            this,   wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnAbout,           this,   wxID_ABORT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnLoadPerspective, this,   ID_LoadPerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnSavePerspective, this,   ID_SavePerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnResetPerspective, this,  ID_ResetPerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnStepInto,        this,   ID_StepInto);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnStepOver,        this,   ID_StepOver);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnStepOut,         this,   ID_StepOut);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnRun,             this,   ID_Run);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnToggleBreakpoint, this,  ID_ToggleBreakpoint);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnRemoveBreakpoint, this,  ID_RemoveBreakpoint);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ArietisFrame::OnShowMemory,      this,   ID_ShowMemory);
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
    wxToolBar *tbDebug = CreateToolBar(wxTB_FLAT | wxTB_TEXT | wxTB_NOICONS, ID_ToolbarDebug, "Debug");

    //wxBitmap bitmap(16, 15);
    //tbDebug->SetMargins(5, 0);

    tbDebug->AddControl(new wxButton(tbDebug, ID_StepInto, "Step Into", wxDefaultPosition, 
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_StepOver, "Step Over", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_StepOut, "Step Out", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_Run, "Run", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddSeparator();
    //tbDebug->AddStretchableSpace();
    m_toggleTrace = new MySwitch(tbDebug, ID_ToolbarToggleTrace, "Trace", wxSize(50, -1));
    m_toggleCRTEntry = new MySwitch(tbDebug, ID_ToolbarToggleCRTEntry, "crt_ent", wxSize(50, -1));
    m_toggleSkipDllEntry = new MySwitch(tbDebug, ID_ToolbarToggleSkipDllEntry, "No_DLL", wxSize(50, -1));
    m_toggleTaint = new MySwitch(tbDebug, ID_ToolbarToggleTaint, "Taint", wxSize(50, -1));
    tbDebug->AddControl(m_toggleTrace);
    tbDebug->AddSeparator();
    tbDebug->AddControl(m_toggleCRTEntry);
    tbDebug->AddSeparator();
    tbDebug->AddControl(m_toggleSkipDllEntry);
    tbDebug->AddSeparator();
    tbDebug->AddControl(m_toggleTaint);

    tbDebug->Realize();

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnStepInto,   this, ID_StepInto);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnStepOver,   this, ID_StepOver);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnStepOut,    this, ID_StepOut);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnRun,        this, ID_Run);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnToggleTraceClicked, 
        this, ID_ToolbarToggleTrace);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnToggleCRTEntryClicked, 
        this, ID_ToolbarToggleCRTEntry);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnToggleSkipDllEntryClicked, 
        this, ID_ToolbarToggleSkipDllEntry);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ArietisFrame::OnToggleTaintClicked,
        this, ID_ToolbarToggleTaint);
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


void ArietisFrame::ShowInMemory( u32 addr )
{
    Memory *mem = m_emulator->Proc()->Mem();
    std::vector<SectionInfo>    secInfo = mem->GetMemoryInfo();
    bool found = false;
    for (auto &sec : secInfo) {
        if (addr < sec.base || addr >= sec.base + sec.size) 
            continue;
        const Section *section = mem->GetSection(sec.base);
        Assert(section->Contains(addr));

        SectionContext ctx(sec, m_emulator->Proc()->GetModuleInfo(sec.Module));
        m_memDataPanel->UpdateData(section, ctx);
        m_memDataPanel->SelectAddress(addr);

        found = true;
        break;
    }
    if (!found) {
        wxMessageBox(wxString::Format("Invalid address %08x", addr));
    }
}


void ArietisFrame::ReportBusy( bool isBusy )
{
    m_statusbar->SetStatusText(isBusy ? "Busy" : "", Statusbar_Busy);
}

void ArietisFrame::OnProcessLoaded( LPCSTR path )
{
    m_statusbar->SetStatusText(path, Statusbar_Path);
}

void ArietisFrame::OnToggleTraceClicked( wxCommandEvent &event )
{
    bool beingEnabled = !m_engine->GetTracer()->IsEnabled();
    m_engine->GetTracer()->Enable(beingEnabled);
    m_statusbar->SetStatusText(beingEnabled ? "Tracing" : "", Statusbar_Tracing);
    m_toggleTrace->SetOn(beingEnabled);
    m_engine->SaveArchive();
}

void ArietisFrame::OnToggleCRTEntryClicked( wxCommandEvent &event )
{
    m_archive->BreakOnCRTEntry = !m_archive->BreakOnCRTEntry;
    m_toggleCRTEntry->SetOn(m_archive->BreakOnCRTEntry);
    m_engine->SaveArchive();
}

void ArietisFrame::OnToggleSkipDllEntryClicked( wxCommandEvent &event )
{
    m_archive->SkipDllEntries = !m_archive->SkipDllEntries;
    m_toggleSkipDllEntry->SetOn(m_archive->SkipDllEntries);
    m_engine->SaveArchive();
}

void ArietisFrame::OnArchiveLoaded( Archive *arc )
{
    m_archive = arc;
    m_statusbar->SetStatusText(m_archive->IsTracerEnabled ? "Tracing" : "", Statusbar_Tracing);
    m_toggleTrace->SetOn(m_archive->IsTracerEnabled);
    m_toggleCRTEntry->SetOn(m_archive->BreakOnCRTEntry);
    m_toggleSkipDllEntry->SetOn(m_archive->SkipDllEntries);
    m_toggleTaint->SetOn(m_archive->IsTaintEnabled);
}

void ArietisFrame::OnToggleTaintClicked( wxCommandEvent &event )
{
    bool beingEnabled = !m_engine->GetTaintEngine()->IsEnabled();
    m_engine->GetTaintEngine()->Enable(beingEnabled);
    m_toggleTaint->SetOn(beingEnabled);
    m_engine->SaveArchive();
}

void ArietisFrame::OnShowMemory( wxCommandEvent &event )
{
    wxString str = wxGetTextFromUser("Input address");
    unsigned long addr = 0;
    if (!str.ToULong(&addr, 16)) {
        wxMessageBox("Invalid address input %s", str);
        return;
    }

    ShowInMemory((u32) addr);
}
