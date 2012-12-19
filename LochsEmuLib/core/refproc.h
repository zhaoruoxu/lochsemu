#pragma once

#ifndef __CORE_REFPROC_H__
#define __CORE_REFPROC_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()


typedef NTSTATUS (WINAPI* FuncNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );
typedef NTSTATUS (WINAPI* FuncNtQueryInformationThread) (
    HANDLE ThreadHandle,
    int ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength,
    PULONG ReturnLength
    );

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus; 
    PVOID TebBaseAddress; 
    u32 Reserved[5]; // size should be 0x1C, 
    //  http://undocumented.ntinternals.net/UserMode/Undocumented%20Functions/NT%20Objects/Thread/THREAD_INFORMATION_CLASS.html
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

void GetThreadInfo(HANDLE hProcess, HANDLE hThread, NT_TIB *tib);
u32 GetProcessPebAddress(HANDLE hProcess);


class LX_API RefProcess {
public:

    struct ModuleLoadInfo {
        std::string Path;
        u32         ImageBase;
        ModuleLoadInfo(const std::string &p, u32 b) { Path = p; ImageBase = b; }
    };

    RefProcess();
    ~RefProcess();

    void        Initialize(Emulator *emu);
    u32         GetMainStackBase(void);
    u32         GetMainStackLimit(void);
    u32         GetPebAddress(void);
    ModuleLoadInfo *        GetModuleLoadInfo(LPCSTR name);
    PROCESS_INFORMATION *   GetProcessInformation() { return &m_pi; }
    DEBUG_EVENT *           GetDebugEvent() { return &m_event; }

    void        GetMainContext(CONTEXT *ctx, DWORD flags);
    void        SetMainContext(CONTEXT *ctx);
    u8          SetInt3(u32 address);
    void        RestoreInt3(u32 address, u8 orig);
    void        SetTF();

private:
    void        CheckInitialized(void);
    void        CreateRefProcess(void);
    void        GetContext(void);



private:
    std::vector<ModuleLoadInfo> m_modules;
    PROCESS_INFORMATION     m_pi;
    DEBUG_EVENT             m_event;
    bool                    m_initialized;
    Emulator *              m_emu;
    bool                    m_createNewConsole;
    NT_TIB                  m_mainTib;
    u32                     m_pebAddress;
};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_REFPROC_H__
