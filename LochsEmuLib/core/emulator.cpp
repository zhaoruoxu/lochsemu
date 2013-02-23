#include "stdafx.h"
#include "emulator.h"
#include "pemodule.h"
#include "processor.h"
#include "process.h"
#include "peloader.h"
#include "pluginmgr.h"
#include "config.h"
#include "refproc.h"

BEGIN_NAMESPACE_LOCHSEMU()


Emulator::Emulator() 
{
    m_memory        = NULL;
    m_process       = NULL;
    m_loader        = NULL;
    m_refProcess    = NULL;
    m_loaded        = false;
}

Emulator::~Emulator()
{
    SAFE_DELETE(m_process);
    SAFE_DELETE(m_refProcess);
    SAFE_DELETE(m_memory);
    SAFE_DELETE(m_loader);
    SAFE_DELETE(m_pluginManager);
}

LxResult Emulator::Initialize()
{
    LxDebug("Initializing Emulator\n");
    B( m_memory         = new Memory() );
    B( m_loader         = new PeLoader() );
    B( m_process        = new Process() );
    B( m_refProcess     = new RefProcess() );
    B( m_pluginManager  = new PluginManager() );
    m_loaded                = false;

    V( m_loader->Initialize(this) );
    V( m_pluginManager->Initialize() );

    RET_SUCCESS();
}

LxResult Emulator::LoadModule( int argc, LPSTR argv[] )
{
    Assert(argc >= 1);
    strcpy(m_path, argv[0]);
    ConstructCmdline(argc, argv);
    if (!LxFileExists(argv[0]))
        RET_FAIL(LX_RESULT_FILE_NOT_EXIST);

    Assert(m_loader);

    V( m_pluginManager->OnProcessPreLoad(m_loader) );

    m_refProcess->Initialize(this);

    V( m_loader->Load(argv[0]) );

    V( Proc()->Initialize(this) );

    m_hModule = LoadLibraryExA(Path(), NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (m_hModule == NULL) {
        LxFatal("LoadLibraryEx() failed\n");
    }

    m_loaded = true;

    V(m_pluginManager->OnProcessPostLoad(m_loader));

    RET_SUCCESS();
}

void Emulator::Run()
{
    V( m_process->Run() );
}

void Emulator::Reset()
{
    SAFE_DELETE(m_process);
    SAFE_DELETE(m_refProcess);
    SAFE_DELETE(m_memory);
    SAFE_DELETE(m_loader);
    SAFE_DELETE(m_pluginManager);
    ZeroMemory(m_path, MAX_PATH);
    ZeroMemory(m_cmdline, LX_CMDLINE_SIZE);
    m_loader    = false;
    m_hModule   = NULL;
}

Processor * Emulator::GetProcessorByThreadID( ThreadID id )
{
    if (m_process == NULL) return NULL;
    Thread *th = m_process->GetThread(id);
    if (th == NULL) return NULL;
    return th->CPU();
}

u32 Emulator::InquireStackBase( void )
{
    if (m_refProcess) return m_refProcess->GetMainStackBase();
    return LxConfig.GetUint("Emulator", "DefaultStackBase", 0x600000);
}

u32 Emulator::InquirePebAddress( void )
{
    if (m_refProcess) return m_refProcess->GetPebAddress();
    return LxConfig.GetUint("Emulator", "DefaultPebAddress", 0x7FFDF000);
}


u32 Emulator::InquireStackLimit( void )
{
    Assert(m_refProcess);
    return m_refProcess->GetMainStackLimit();
}

HMODULE Emulator::TranslateModule( HMODULE hModule )
{
    if (m_loader->GetModuleInfo(0)->ImageBase == (u32) hModule) {
        return m_hModule;
    }
    return hModule;
}

void Emulator::ConstructCmdline( int argc, LPSTR argv[] )
{
    ZeroMemory(m_cmdline, LX_CMDLINE_SIZE);
    sprintf(m_cmdline, "\"%s\"", argv[0]);
    for (int i = 1; i < argc; i++) {
        strcat(m_cmdline, " ");
        strcat(m_cmdline, argv[i]);
    }

    LxWarning("CMDLINE:\n");
    LxWarning("%s\n", m_cmdline);
}

void Emulator::Terminate()
{
    m_process->Terminate();
}


END_NAMESPACE_LOCHSEMU()


