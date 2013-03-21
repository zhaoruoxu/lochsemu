#include "stdafx.h"
#include "mainframe.h"

#include "logpanel.h"
#include "cpupanel.h"
#include "tracepanel.h"
#include "contextpanel.h"
#include "mempanel.h"
#include "bpspanel.h"
#include "myswitch.h"
#include "msgpanel.h"
#include "statpanel.h"

#include "utilities.h"
#include "engine.h"
#include "dbg/tracer.h"
#include "plugin/plugin.h"

#include "instruction.h"
#include "processor.h"

#include "buildver.h"

ProphetFrame::ProphetFrame(ProEngine *engine, Emulator *emu)
    : m_engine(engine), wxFrame(NULL, wxID_ANY, 
    wxString::Format("Prophet %x build %d", ProphetVersion, PROPHET_BUILD_VERSION), 
    wxDefaultPosition, wxSize(850, 850), wxDEFAULT_FRAME_STYLE),
    m_statusTimer(this, ID_StatusTimer)
{
    m_archive   = m_engine->GetArchive();
    m_tracer    = m_engine->GetTracer();
    m_taint     = m_engine->GetTaintEngine();
    m_plugins   = m_engine->GetPluginManager();

    InitMisc();
    InitUI();

    m_currProcessor = NULL;
    m_isProcLoaded  = false;
    m_isbusy        = true;
    m_engine->SetGuiFrame(this);
    NotifyMainThread(); // so that main thread can go on
}

ProphetFrame::~ProphetFrame()
{
    m_auiManager.UnInit();
}

void ProphetFrame::InitMisc()
{
}

void ProphetFrame::InitUI()
{
    InitMenu();
    InitStatusBar();
    InitToolbars();

    Bind(wxEVT_CLOSE_WINDOW, &ProphetFrame::OnClose, this, wxID_ANY);

    m_auiManager.SetManagedWindow(this);

    m_cpuPanel      = new CpuPanel(this, m_engine);
    m_contextPanel  = new ContextPanel(this);
    m_tracePanel    = new CompositeTracePanel(this, m_contextPanel);
    m_memDataPanel  = new MemDataPanel(this, m_engine);
    m_memInfoPanel  = new MemInfoPanel(this, m_memDataPanel);
    m_bpsPanel      = new BreakpointsPanel(this);
    m_msgPanel      = new MessagePanel(this, m_engine);
    m_statPanel     = new StatPanel(this, m_engine);
    
    const long noteStyle = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | 
        wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_WINDOWLIST_BUTTON | 
        wxAUI_NB_TAB_EXTERNAL_MOVE;
    m_nbMain        = new wxAuiNotebook(this, ID_NbMain,    wxDefaultPosition, wxSize(400, 400), noteStyle);
    m_nbContext     = new wxAuiNotebook(this, ID_NbContext, wxDefaultPosition, wxSize(200, 400), noteStyle);
    m_nbSections    = new wxAuiNotebook(this, ID_NbSections, wxDefaultPosition, wxSize(400, 200), noteStyle);
    //m_nbTrace       = new wxAuiNotebook(this, ID_NbTrace,   wxDefaultPosition, wxSize(200, 200), noteStyle);
    //m_nbMemory      = new wxAuiNotebook(this, ID_NbMemory,  wxDefaultPosition, wxSize(400, 200), noteStyle);
    //m_nbStat        = new wxAuiNotebook(this, ID_NbStat,    wxDefaultPosition, wxSize(200, 200), noteStyle);

    m_nbMain->AddPage(m_cpuPanel, "CPU");
    m_nbContext->AddPage(m_contextPanel, "Context");
    m_nbContext->AddPage(m_msgPanel, "Message");
    m_nbSections->AddPage(m_memInfoPanel, "Sections");
    m_nbSections->AddPage(m_bpsPanel, "Breakpoints");
    //m_nbTrace->AddPage(m_tracePanel, "Trace");
    //m_nbMemory->AddPage(m_memDataPanel, "Memory");
    //m_nbStat->AddPage(m_statPanel, "Stat");

    m_auiManager.AddPane(m_nbMain, wxAuiPaneInfo().Name("Main").CenterPane());
    m_auiManager.AddPane(m_nbContext, wxAuiPaneInfo().Name("Context").Right());
    m_auiManager.AddPane(m_nbSections, wxAuiPaneInfo().Name("Sections").Bottom().Position(0).Row(1));
    m_auiManager.AddPane(m_tracePanel, wxAuiPaneInfo().Name("Trace").Caption("Trace").Bottom().Position(1).Row(1));
    m_auiManager.AddPane(m_memDataPanel, wxAuiPaneInfo().Name("Memory").Caption("Memory").Bottom().Position(0).Row(0));
    m_auiManager.AddPane(m_statPanel, wxAuiPaneInfo().Name("Stat").Caption("Stat").Bottom().Position(1).Row(0));
    
    Bind(wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, &ProphetFrame::OnAllowDND, this, wxID_ANY);
    
    m_auiManager.Update();
    Centre();

    m_defaultPerspective = m_auiManager.SavePerspective();

    if (g_config.GetInt("General", "AutoLoadPerpective", 1) != 0) {
        OnLoadPerspective(wxCommandEvent());
    }
    
}

void ProphetFrame::InitMenu()
{
    m_menuFile = new wxMenu;
    m_menuFile->Append(wxID_EXIT, "Exit\tCtrl-Q");

    m_menuView = new wxMenu;
    m_menuView->Append(ID_LoadPerspective, "Load perspective");
    m_menuView->Append(ID_SavePerspective, "Save perspective");
    m_menuView->Append(ID_ResetPerspective, "Reset perspective");

    m_menuDebug = new wxMenu;
    m_menuDebug->Append(ID_Run, "Run\tF5");
    m_menuDebug->Append(ID_StepOver, "Step Over\tF10");
    m_menuDebug->Append(ID_StepInto, "Step Into\tF11");
    m_menuDebug->Append(ID_StepOut, "Step Out\tShift-F10");
    m_menuDebug->AppendSeparator();
    m_menuDebug->Append(ID_ToggleBreakpoint, "Toggle Breakpoint\tF2");
    m_menuDebug->Append(ID_RemoveBreakpoint, "Remove Breakpoint\tF3");
    m_menuDebug->AppendSeparator();
    m_menuDebug->Append(ID_ShowMemory, "Show memory by address\tCtrl-M");

    m_menuHelp = new wxMenu;
    m_menuHelp->Append(wxID_ABORT, "&About");

    m_menuPlugins = new wxMenu;
    const int N = m_plugins->GetNumPlugins();
    for (int i = 0; i < N; i++) {
        m_menuPlugins->AppendCheckItem(ID_PluginCheckEnable+i, m_plugins->GetPlugin(i)->GetName());
        Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnPluginCheckEnable, this, ID_PluginCheckEnable+i);
    }

    m_mainMenu = new wxMenuBar;
    m_mainMenu->Append(m_menuFile, "&File");
    m_mainMenu->Append(m_menuView, "&View");
    m_mainMenu->Append(m_menuDebug, "&Debug");
    m_mainMenu->Append(m_menuPlugins, "&Plugins");
    m_mainMenu->Append(m_menuHelp, "&Help");
    SetMenuBar(m_mainMenu);

    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnExit,            this,   wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnAbout,           this,   wxID_ABORT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnLoadPerspective, this,   ID_LoadPerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnSavePerspective, this,   ID_SavePerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnResetPerspective,this,  ID_ResetPerspective);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnStepInto,        this,   ID_StepInto);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnStepOver,        this,   ID_StepOver);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnStepOut,         this,   ID_StepOut);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnRun,             this,   ID_Run);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnToggleBreakpoint,this,  ID_ToggleBreakpoint);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnRemoveBreakpoint,this,  ID_RemoveBreakpoint);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ProphetFrame::OnShowMemory,      this,   ID_ShowMemory);
    
}

enum StatusbarText {
    Statusbar_HelpString = 0,
    Statusbar_Path,
    Statusbar_Tracing,
    Statusbar_Busy,
    Statusbar_Cpu,
    Statusbar_Mem,
    Statusbar_Threads,
};

void ProphetFrame::InitStatusBar()
{
    static const int Widths[] = { 240, -1, 50, 40, 70, 100, 75 };
    m_statusbar = CreateStatusBar(_countof(Widths));
    m_statusbar->SetStatusWidths(_countof(Widths), Widths);

    m_statusTimer.Start(g_config.GetInt("General", "StatusUpdateInterval", 1000));
    Bind(wxEVT_TIMER, &ProphetFrame::OnStatusTimer, this, ID_StatusTimer);
}


void ProphetFrame::InitToolbars()
{
    wxToolBar *tbDebug = CreateToolBar(wxTB_FLAT | wxTB_TEXT | wxTB_NOICONS, ID_ToolbarDebug, "Debug");


    tbDebug->AddControl(new wxButton(tbDebug, ID_StepInto, "Step Into", wxDefaultPosition, 
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_StepOver, "Step Over", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_StepOut, "Step Out", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddControl(new wxButton(tbDebug, ID_Run, "Run", wxDefaultPosition,
        wxSize(60, -1), wxBORDER_NONE));
    tbDebug->AddSeparator();
    m_toggleTrace = new MySwitch(tbDebug, ID_ToolbarToggleTrace, "Trace", wxSize(50, -1));
    m_toggleTaint = new MySwitch(tbDebug, ID_ToolbarToggleTaint, "Taint", wxSize(50, -1));
    tbDebug->AddControl(m_toggleTrace);
    tbDebug->AddSeparator();
    tbDebug->AddControl(m_toggleTaint);

    tbDebug->Realize();

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ProphetFrame::OnStepInto,   this, ID_StepInto);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ProphetFrame::OnStepOver,   this, ID_StepOver);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ProphetFrame::OnStepOut,    this, ID_StepOut);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ProphetFrame::OnRun,        this, ID_Run);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ProphetFrame::OnToggleTraceClicked, 
        this, ID_ToolbarToggleTrace);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ProphetFrame::OnToggleTaintClicked,
        this, ID_ToolbarToggleTaint);
}


//////////////////////////////////////////////////////////////////////////
// Event handlers
//////////////////////////////////////////////////////////////////////////
void ProphetFrame::OnExit( wxCommandEvent &event )
{
    Close();
}

void ProphetFrame::OnAbout( wxCommandEvent &event )
{
    wxMessageBox("Prophet GUI application", "Prophet", wxOK | wxICON_INFORMATION);
}

void ProphetFrame::OnClose( wxCloseEvent &event )
{
//     if (event.CanVeto()) {
//         if (wxMessageBox("Closing this window will continue emulation without Prophet.\n\nYou sure?", "Quit", wxICON_QUESTION | wxYES_NO) != wxYES) {
//             event.Veto();
//             return;
//         }
//     }
    if (g_config.GetInt("General", "AutoSavePerspective", 1) != 0) {
        OnSavePerspective(wxCommandEvent());
    }
    Destroy();
    m_engine->Terminate();
}

void ProphetFrame::OnSavePerspective( wxCommandEvent &event )
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

void ProphetFrame::OnLoadPerspective( wxCommandEvent &event )
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

void ProphetFrame::OnResetPerspective( wxCommandEvent &event )
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

void ProphetFrame::OnStatusTimer( wxTimerEvent &event )
{
    m_statusbar->SetStatusText(m_pathText, Statusbar_Path);
    m_statusbar->SetStatusText(wxString::Format("CPU: %d%%", GetCpuUsage()), Statusbar_Cpu);
    m_statusbar->SetStatusText(wxString::Format("MEM: %d MB", GetMemUsage()), Statusbar_Mem);
    m_statusbar->SetStatusText(wxString::Format("Threads: %d", GetThreadCount()), Statusbar_Threads);
}

void ProphetFrame::OnStepInto( wxCommandEvent &event )
{
    if (m_isbusy) return;
    m_engine->GetDebugger()->OnStepInto();
}

void ProphetFrame::OnRun( wxCommandEvent &event )
{
    if (m_isbusy) return;
    OnSavePerspective(wxCommandEvent());
    m_engine->GetDebugger()->OnRun();
}

void ProphetFrame::OnStepOver( wxCommandEvent &event )
{
    if (m_isbusy) return;
    m_engine->GetDebugger()->OnStepOver();
}

void ProphetFrame::OnStepOut( wxCommandEvent &event )
{
    if (m_isbusy) return;
    m_engine->GetDebugger()->OnStepOut();
}

void ProphetFrame::OnToggleBreakpoint( wxCommandEvent &event )
{
    if (m_isbusy) return;
    u32 eip = m_cpuPanel->GetSelectedEip();
    if (eip == 0) eip = m_cpuPanel->GetCurrentEip();
    Assert(eip != 0);
    m_engine->GetDebugger()->ToggleBreakpoint(eip);
    m_bpsPanel->UpdateData(m_engine);
    m_cpuPanel->Refresh();
}

void ProphetFrame::OnRemoveBreakpoint( wxCommandEvent &event )
{
    if (m_isbusy) return;
    u32 eip = m_cpuPanel->GetSelectedEip();
    if (eip == 0) eip = m_cpuPanel->GetCurrentEip();
    Assert(eip != 0);
    m_engine->GetDebugger()->RemoveBreakpoint(eip);
    m_bpsPanel->UpdateData(m_engine);
    m_cpuPanel->Refresh();
}

void ProphetFrame::PreExecSingleStepCallback( const Processor *cpu, const Instruction *inst )
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


void ProphetFrame::ShowInMemory( u32 addr )
{
    Assert(m_currProcessor);
    Memory *mem = LxEmulator.Mem();
    std::vector<SectionInfo>    secInfo = mem->GetMemoryInfo();
    bool found = false;
    for (auto &sec : secInfo) {
        if (addr < sec.base || addr >= sec.base + sec.size) 
            continue;
        const Section *section = mem->GetSection(sec.base);
        Assert(section->Contains(addr));

        SectionContext ctx(sec, LxEmulator.Proc()->GetModuleInfo(sec.Module));
        m_memDataPanel->UpdateData(section, ctx);
        m_memDataPanel->SelectAddress(addr, 4);

        found = true;
        break;
    }
    if (!found) {
        wxMessageBox(wxString::Format("Invalid address %08x", addr));
    }
}


void ProphetFrame::ReportBusy( bool isBusy )
{
    m_isbusy = isBusy;
    m_statusbar->SetStatusText(isBusy ? "Busy" : "Idle", Statusbar_Busy);
    m_statPanel->ReportBusy(isBusy);
}

void ProphetFrame::OnProcessLoaded( LPCSTR path )
{
    m_pathText      = path;
    m_isProcLoaded  = true;
    m_statPanel->Start();
}

void ProphetFrame::OnToggleTraceClicked( wxCommandEvent &event )
{
    if (m_isbusy) return;
    bool beingEnabled = !m_engine->GetTracer()->IsEnabled();
    m_engine->GetTracer()->Enable(beingEnabled);
    m_statusbar->SetStatusText(beingEnabled ? "Tracing" : "", Statusbar_Tracing);
    m_toggleTrace->SetOn(beingEnabled);
    m_engine->SaveArchive();
}

// void ProphetFrame::OnToggleCRTEntryClicked( wxCommandEvent &event )
// {
//     m_archive->BreakOnCRTEntry = !m_archive->BreakOnCRTEntry;
//     m_toggleCRTEntry->SetOn(m_archive->BreakOnCRTEntry);
//     m_engine->SaveArchive();
// }
// 
// void ProphetFrame::OnToggleSkipDllEntryClicked( wxCommandEvent &event )
// {
//     m_archive->SkipDllEntries = !m_archive->SkipDllEntries;
//     m_toggleSkipDllEntry->SetOn(m_archive->SkipDllEntries);
//     m_engine->SaveArchive();
// }

void ProphetFrame::OnArchiveLoaded( Archive *arc )
{
    m_archive = arc;
    OnUpdate();
}

void ProphetFrame::OnToggleTaintClicked( wxCommandEvent &event )
{
    if (m_isbusy) return;
    bool beingEnabled = !m_engine->GetTaintEngine()->IsEnabled();
    m_engine->GetTaintEngine()->Enable(beingEnabled);
    m_toggleTaint->SetOn(beingEnabled);
    m_engine->SaveArchive();
}

void ProphetFrame::OnShowMemory( wxCommandEvent &event )
{
    if (m_isbusy) return;
    wxString str = wxGetTextFromUser("Input address");
    if (str.IsEmpty()) return;

    unsigned long addr = 0;
    if (!str.ToULong(&addr, 16)) {
        wxMessageBox("Invalid address input " + str);
        return;
    }

    ShowInMemory((u32) addr);
}

void ProphetFrame::OnPluginCheckEnable( wxCommandEvent &event )
{
    if (m_isbusy) return;
    bool isChecked = m_menuPlugins->IsChecked(event.GetId());
    m_plugins->GetPlugin(event.GetId() - ID_PluginCheckEnable)->Enable(isChecked);
    m_engine->SaveArchive();
}

void ProphetFrame::OnUpdate()
{
    m_statusbar->SetStatusText(m_tracer->IsEnabled() ? "Tracing" : "", Statusbar_Tracing);
    m_toggleTrace->SetOn(m_tracer->IsEnabled());
    m_toggleTaint->SetOn(m_taint->IsEnabled());

    for (int i = 0; i < m_plugins->GetNumPlugins(); i++)
        m_menuPlugins->Check(ID_PluginCheckEnable+i, m_plugins->GetPlugin(i)->IsEnabled());
}

void ProphetFrame::ShowMessage( const Message *msg )
{
    m_msgPanel->UpdateData(msg);
}

void ProphetFrame::OnAllowDND( wxAuiNotebookEvent &event )
{
    event.Allow();
}

void ProphetFrame::OnProcessPostRun( ProcessPostRunEvent &event )
{
    m_statPanel->Stop();
}

void ProphetFrame::OnPreExecute( PreExecuteEvent &event )
{
    m_currProcessor = event.Cpu;
}
