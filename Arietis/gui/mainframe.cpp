#include "stdafx.h"
#include "mainframe.h"

#include "logpanel.h"
#include "cpupanel.h"
#include "regpanel.h"

#include "utilities.h"
#include "engine.h"

#include "instruction.h"
#include "processor.h"

ArietisFrame::ArietisFrame(ArietisEngine *engine)
    : m_engine(engine), wxFrame(NULL, wxID_ANY, "Arietis", wxDefaultPosition, wxSize(800, 800), 
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
    InitToolbars();

    Bind(wxEVT_CLOSE_WINDOW, &ArietisFrame::OnClose, this, wxID_ANY);

    m_auiManager.SetManagedWindow(this);

    m_logPanel = new LogPanel(this);
    m_cpuPanel = new CpuPanel(this);
    m_regPanel = new RegPanel(this);

    m_auiManager.AddPane(m_cpuPanel, wxAuiPaneInfo().Name("Taint").Caption("CPU").CenterPane());
    m_auiManager.AddPane(m_regPanel, wxAuiPaneInfo().Name("Reg").Caption("Registers").Right());
    m_auiManager.AddPane(m_logPanel, wxAuiPaneInfo().Name("Log").Caption("Log").Bottom());

    m_auiManager.Update();
    m_defaultPerspective = m_auiManager.SavePerspective();
    Centre();
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
    menuDebug->Append(ID_StepInto, "Step Into\tF11");

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
}



void ArietisFrame::InitStatusBar()
{
    static const int Widths[] = { -1, 70, 80, 75 };
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
//     if (event.CanVeto()) {
//         if (wxMessageBox("Really??", "Don't", wxICON_QUESTION | wxYES_NO) != wxYES) {
//             event.Veto();
//             return;
//         }
//     }
    
    Destroy();
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
    m_statusbar->SetStatusText(wxString::Format("CPU: %d%%", GetCpuUsage()), 1);
    m_statusbar->SetStatusText(wxString::Format("MEM: %d MB", GetMemUsage()), 2);
    m_statusbar->SetStatusText(wxString::Format("Threads: %d", GetThreadCount()), 3);
}

void ArietisFrame::OnStepInto( wxCommandEvent &event )
{
    m_engine->OnStepInto();
}

void ArietisFrame::DebugLog( const wxString &s )
{
    m_logPanel->Log(s);
}

void ArietisFrame::DebugStepCallback( const Processor *cpu, const Instruction *inst )
{
    m_regPanel->SetValues(cpu);
    m_cpuPanel->OnPtrChange(cpu->EIP);
    //DebugLog(wxString::Format("%s", inst->Main.CompleteInstr));
}
