#include "stdafx.h"
#include "refproc.h"
#include "config.h"
#include "emulator.h"

BEGIN_NAMESPACE_LOCHSEMU()

static const u8     CC = 0xcc;

RefProcess::RefProcess()
{
    m_initialized = false;
}

RefProcess::~RefProcess()
{

}

void RefProcess::Initialize( Emulator *emu )
{
    Assert(emu);
    m_emu = emu;
    ZeroMemory(&m_pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&m_event, sizeof(DEBUG_EVENT));
    m_createNewConsole = LxConfig.GetInt("ReferenceProcess", "CreateNewConsole", 0) != 0;

    CreateRefProcess();
    GetContext();
    m_initialized = true;
}

void RefProcess::CreateRefProcess( void )
{
    char path[MAX_PATH];
    STARTUPINFOA si = {0};
    si.cb = sizeof(STARTUPINFOA);
    LPCSTR cmdLine = LxEmulator.CmdLine();
    char cmd[LX_CMDLINE_SIZE];
    strcpy(cmd, cmdLine);
    BOOL okay = CreateProcessA(m_emu->Path(), cmd, NULL, NULL, FALSE,
        m_createNewConsole ? DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE : DEBUG_ONLY_THIS_PROCESS,
        NULL, NULL, &si, &m_pi);
    if (!okay) {
        LxFatal("Cannot create reference process\n");
    }
    WaitForDebugEvent(&m_event, INFINITE);
    if (m_event.dwDebugEventCode != CREATE_PROCESS_DEBUG_EVENT) {
        LxFatal("Unexpected debug event %d\n", m_event.dwDebugEventCode);
    }

    GetFinalPathNameByHandleA(m_event.u.CreateProcessInfo.hFile, path, MAX_PATH, FILE_NAME_NORMALIZED);
    m_modules.push_back(ModuleLoadInfo(path, (u32) m_event.u.CreateProcessInfo.lpBaseOfImage));
    ContinueDebugEvent(m_event.dwProcessId, m_event.dwThreadId, DBG_CONTINUE);

    while (WaitForDebugEvent(&m_event, INFINITE)) {
        if (m_event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
            m_event.u.Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT) 
        {
            LxInfo("RefProcess system breakpoint at %08x\n",
                m_event.u.Exception.ExceptionRecord.ExceptionAddress);
            break;
        } else if (m_event.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT) {
            GetFinalPathNameByHandleA(m_event.u.LoadDll.hFile, path, MAX_PATH, FILE_NAME_NORMALIZED);
            LxInfo("RefProcess load dll: %s, at address %08x\n", path+4, (u32) m_event.u.LoadDll.lpBaseOfDll);
            m_modules.push_back(ModuleLoadInfo(path+4, (u32) m_event.u.LoadDll.lpBaseOfDll));
        }
        ContinueDebugEvent(m_event.dwProcessId, m_event.dwThreadId, DBG_CONTINUE);
    }
}

void RefProcess::GetContext( void )
{
    GetThreadInfo(m_pi.hProcess, m_pi.hThread, &m_mainTib);
    m_pebAddress = GetProcessPebAddress(m_pi.hProcess);
}

u32 RefProcess::GetMainStackBase( void )
{
    CheckInitialized();
    return reinterpret_cast<u32>(m_mainTib.StackBase);
}

u32 RefProcess::GetMainStackLimit( void )
{
    CheckInitialized();
    return reinterpret_cast<u32>(m_mainTib.StackLimit);
}

u32 RefProcess::GetPebAddress( void )
{
    CheckInitialized();
    return m_pebAddress;
}

void RefProcess::CheckInitialized( void )
{
    if (!m_initialized) {
        LxFatal("Reference process not initialized\n");
    }
}

RefProcess::ModuleLoadInfo * RefProcess::GetModuleLoadInfo( LPCSTR name )
{
    for (uint i = 0; i < m_modules.size(); i++) {
        if (StrStrIA(m_modules[i].Path.c_str(), name)) {
            return &m_modules[i];
        }
    }
    return NULL;
}

u8 RefProcess::SetInt3( u32 address )
{
    u8 orig;
    B( ReadProcessMemory(m_pi.hProcess, (LPCVOID) address, &orig, 1, NULL) );
    B( WriteProcessMemory(m_pi.hProcess, (LPVOID) address, &CC, 1, NULL) );
    return orig;
}

void RefProcess::RestoreInt3( u32 address, u8 orig )
{
    B( WriteProcessMemory(m_pi.hProcess, (LPVOID) address, &orig, 1, NULL) );
}

void RefProcess::SetTF()
{
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_CONTROL;
    B( GetThreadContext(m_pi.hThread, &ctx) );
    ctx.EFlags |= 0x100;
    B( SetThreadContext(m_pi.hThread, &ctx) );
}

void RefProcess::GetMainContext( CONTEXT *ctx, DWORD flags )
{
    ctx->ContextFlags = flags;
    B( GetThreadContext(m_pi.hThread, ctx) );
}

void RefProcess::SetMainContext( CONTEXT *ctx )
{
    B( SetThreadContext(m_pi.hThread, ctx) );
}

void GetThreadInfo( HANDLE hProcess, HANDLE hThread, NT_TIB *tib )
{
    FuncNtQueryInformationThread ntqit = (FuncNtQueryInformationThread)
        GetProcAddress( GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread" );
    if (!ntqit) {
        LxFatal("Unable to load ntdll.dll::NtQueryInformationThread()\n");
    }
    THREAD_BASIC_INFORMATION thrInfo;
    ULONG retLen;
    ntqit(hThread, 0, &thrInfo, sizeof(THREAD_BASIC_INFORMATION), &retLen);
    B( ReadProcessMemory(hProcess, (LPCVOID) thrInfo.TebBaseAddress, tib, sizeof(NT_TIB), NULL) );
}

u32 GetProcessPebAddress( HANDLE hProcess )
{
    PROCESS_BASIC_INFORMATION procInfo;
    ULONG retLen;
    FuncNtQueryInformationProcess ntqip = (FuncNtQueryInformationProcess)
        GetProcAddress( GetModuleHandleA( "ntdll.dll" ), "NtQueryInformationProcess" );
    if (!ntqip) {
        LxFatal("Unable to load ntdll.dll::NtQueryInformationProcess()\n");
    }
    ntqip(hProcess, ProcessBasicInformation, (PVOID) &procInfo,
        sizeof(PROCESS_BASIC_INFORMATION), &retLen);
    return (u32) procInfo.PebBaseAddress;
}


END_NAMESPACE_LOCHSEMU()