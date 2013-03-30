#include "stdafx.h"
#include "winapi.h"
#include "processor.h"
#include "emulator.h"
#include "heap.h"
#include "process.h"
#include "config.h"

BEGIN_NAMESPACE_LOCHSEMU()




uint Pgort90_FindActCtxSectionStringW(Processor *cpu)
{
    RET_VALUE = (u32) FindActCtxSectionStringW(
        (DWORD)     PARAM(0),
        (const GUID *)  PARAM_PTR(1),
        (ULONG)     PARAM(2),
        (LPCWSTR)   PARAM_PTR(3),
        (PACTCTX_SECTION_KEYED_DATA)    PARAM_PTR(4)
        );
    RET_PARAMS(5);
}

uint Kernel32_AreFileApisANSI(Processor *cpu)
{
    RET_VALUE = (BOOL) AreFileApisANSI();
    RET_PARAMS(0);
}

uint Kernel32_AddAtomA(Processor *cpu)
{
	RET_VALUE = (u32) AddAtomA((LPCSTR) PARAM_PTR(0));
	RET_PARAMS(1);
}

uint Kernel32_CloseHandle(Processor *cpu)
{
    HANDLE h = (HANDLE) cpu->GetStackParam32(0);
    cpu->EAX = (u32) CloseHandle(h);

    LxWarning("CloseHandle() is called\n");

    return 1;
}

uint Kernel32_CompareStringW(Processor *cpu)
{
	RET_VALUE = (u32) CompareStringW(
		(LCID)		PARAM(0),
		(DWORD)		PARAM(1),
		(LPCWSTR)	PARAM_PTR(2),
		(int)		PARAM(3),
		(LPCWSTR)	PARAM_PTR(4),
		(int)		PARAM(5)
		);
	RET_PARAMS(6);
}

uint Kernel32_CreateFileA(Processor *cpu)
{
    LPCSTR lpFileName = (LPCSTR) cpu->GetStackParamPtr32(0);
    DWORD dwDesiredAccess = (DWORD) cpu->GetStackParam32(1);
    DWORD dwShareMode = (DWORD) cpu->GetStackParam32(2);
    LPSECURITY_ATTRIBUTES lpSecurity = (LPSECURITY_ATTRIBUTES) cpu->GetStackParamPtr32(3);
    DWORD dwCreation = (DWORD) cpu->GetStackParam32(4);
    DWORD dwFlags = (DWORD) cpu->GetStackParam32(5);
    HANDLE hTemplate = (HANDLE) cpu->GetStackParam32(6);

    SECURITY_ATTRIBUTES security;
    if (lpSecurity != NULL) {
        security.bInheritHandle = lpSecurity->bInheritHandle;
        security.nLength = lpSecurity->nLength;
        security.lpSecurityDescriptor = 
            lpSecurity->lpSecurityDescriptor == NULL ? NULL : 
            (LPVOID) cpu->Mem->GetRawData((u32) lpSecurity->lpSecurityDescriptor);
    }

    cpu->EAX = (u32) CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, 
        lpSecurity == NULL ? lpSecurity : &security,
        dwCreation, dwFlags, hTemplate);
    return 7;
}

uint Kernel32_CreateFileW(Processor *cpu)
{
    RET_VALUE = (u32) CreateFileW(
        (LPCWSTR)       PARAM_PTR(0),
        (DWORD)         PARAM(1),
        (DWORD)         PARAM(2),
        (LPSECURITY_ATTRIBUTES)     PARAM_PTR(3),
        (DWORD)         PARAM(4),
        (DWORD)         PARAM(5),
        (HANDLE)        PARAM(6)
        );
    RET_PARAMS(7);
}

uint kernel32_CreateProcessA(Processor *cpu)
{
	LPSTARTUPINFOA si = (LPSTARTUPINFOA) PARAM_PTR(8);
	si->lpReserved2 = si->lpReserved2 == NULL ? NULL : 
		(LPBYTE) cpu->Mem->GetRawData((u32) si->lpReserved2);
	si->lpReserved = si->lpReserved == NULL ? NULL :
		(LPSTR) cpu->Mem->GetRawData((u32) si->lpReserved);
	si->lpDesktop = si->lpDesktop == NULL ? NULL :
		(LPSTR) cpu->Mem->GetRawData((u32) si->lpDesktop);
	si->lpTitle = si->lpTitle == NULL ? NULL :
		(LPSTR) cpu->Mem->GetRawData((u32) si->lpTitle);
	RET_VALUE = (u32) CreateProcessA(
		(LPCSTR)		PARAM_PTR(0),
		(LPSTR)		PARAM_PTR(1),
		(LPSECURITY_ATTRIBUTES)	PARAM_PTR(2),
		(LPSECURITY_ATTRIBUTES)	PARAM_PTR(3),
		(BOOL)			PARAM(4),
		(DWORD)			PARAM(5),
		(LPVOID)		PARAM_PTR(6),
		(LPCSTR)		PARAM_PTR(7),
		si,
		(LPPROCESS_INFORMATION)	PARAM_PTR(9)
		);
	RET_PARAMS(10);
}

uint Kernel32_CreateSemaphoreA(Processor *cpu)
{
	RET_VALUE = (u32) CreateSemaphoreA(
		(LPSECURITY_ATTRIBUTES) PARAM(0),
		(LONG) PARAM(1),
		(LONG) PARAM(2),
		(LPCSTR) PARAM_PTR(3)
		);
	RET_PARAMS(4);
}

uint Kernel32_CreateThread(Processor *cpu)
{
    LPSECURITY_ATTRIBUTES lpAttrs = (LPSECURITY_ATTRIBUTES) PARAM_PTR(0);
    if (lpAttrs != NULL)
        LxFatal("SecurityAttributes in CreateThread() is non-NULL!\n");

    ThreadInfo ti;
    ti.StackSize    = (u32) PARAM(1);
    ti.EntryPoint   = (u32) PARAM(2);
    ti.ParamPtr     = (u32) PARAM(3);
    ti.Flags        = (u32) PARAM(4);
    ti.Module       = cpu->GetCurrentModule();

    Thread *t = cpu->Proc()->ThreadCreate(ti);
    LPDWORD lpId = (LPDWORD) PARAM_PTR(5);
    if (lpId) *lpId = t->ID;
    RET_VALUE = (u32) t->Handle;
    RET_PARAMS(6);
}

uint Kernel32_DecodePointer(Processor *cpu)
{
    RET_VALUE = (u32) DecodePointer(
        (PVOID) PARAM(0)    /* treat this pointer as a value */
        );
    RET_PARAMS(1);
}

uint Kernel32_DeleteCriticalSection(Processor *cpu)
{
    LPCRITICAL_SECTION x = (LPCRITICAL_SECTION) PARAM_PTR(0);
    DeleteCriticalSection(
        (LPCRITICAL_SECTION)    PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_DeleteFileA(Processor *cpu)
{
    LPCSTR lpFile = (LPCSTR) cpu->GetStackParamPtr32(0);
    cpu->EAX = (u32) DeleteFileA(lpFile);
    return 1;
}

uint Kernel32_DisableThreadLibraryCalls(Processor *cpu)
{
	RET_VALUE = (u32) DisableThreadLibraryCalls(
		(HMODULE)	PARAM(0)
		);
	RET_PARAMS(1);
}

uint Kernel32_EncodePointer(Processor *cpu)
{
    RET_VALUE = (u32) EncodePointer(
        (PVOID) PARAM(0)    /* treat this pointer as a value */
        );
    RET_PARAMS(1);
}

uint Kernel32_EnterCriticalSection(Processor *cpu)
{
    LPCRITICAL_SECTION x = (LPCRITICAL_SECTION) PARAM_PTR(0);
    EnterCriticalSection(
        (LPCRITICAL_SECTION)    PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_ExitProcess( Processor *cpu )
{
    cpu->Terminate(0);
    return 0;
}

uint Kernel32_FindActCtxSectionStringW(Processor *cpu)
{
    RET_VALUE = (u32) FindActCtxSectionStringW(
        (DWORD)     PARAM(0),
        (const GUID *)  PARAM_PTR(1),
        (ULONG)     PARAM(2),
        (LPCWSTR)   PARAM_PTR(3),
        (PACTCTX_SECTION_KEYED_DATA)    PARAM_PTR(4)
        );
    RET_SUCCESS();
}

uint Kernel32_FindAtomA(Processor *cpu)
{
	RET_VALUE = (u32) FindAtomA((LPCSTR) PARAM_PTR(0));
	RET_PARAMS(1);
}

uint Kernel32_FindClose(Processor *cpu)
{
	RET_VALUE = (u32) FindClose(
		(HANDLE)	PARAM(0));
	RET_PARAMS(1);
}

uint Kernel32_FindFirstFileW(Processor *cpu)
{
	RET_VALUE = (u32) FindFirstFileW(
		(LPCWSTR)	PARAM_PTR(0),
		(LPWIN32_FIND_DATAW)	PARAM_PTR(1));
	RET_PARAMS(2);
}

uint Kernel32_FindResourceExA(Processor *cpu)
{
	RET_VALUE = (u32) FindResourceExA(
		(HMODULE)	PARAM(0),
		(LPCSTR)	PARAM_PTR(1),
		(LPCSTR)	PARAM_PTR(2),
		(WORD)		PARAM(3)
		);
	RET_PARAMS(4);
}

uint Kernel32_FlsAlloc(Processor *cpu)
{
    PFLS_CALLBACK_FUNCTION p = (PFLS_CALLBACK_FUNCTION) cpu->GetStackParam32(0);
    cpu->SetCallbackEntry(LX_CALLBACK_FLS, (u32) p);
    RET_VALUE = (u32) FlsAlloc(LxFlsCallbackFunction);
    RET_PARAMS(1);
}

uint Kernel32_FlsFree(Processor *cpu)
{
    RET_VALUE = (u32) FlsFree(
        (DWORD) PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_FlsGetValue(Processor *cpu)
{
    RET_VALUE = (u32) FlsGetValue(
        (DWORD) PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_FlsSetValue(Processor *cpu)
{
    RET_VALUE = (u32) FlsSetValue(
        (DWORD)     PARAM(0),
        (PVOID)     PARAM(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_FlushConsoleInputBuffer(Processor *cpu)
{
    RET_VALUE = (u32) FlushConsoleInputBuffer(
        (HANDLE)    PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_FreeEnvironmentStringsW(Processor *cpu)
{
    u32 base = (u32) PARAM(0);
    V( cpu->Mem->Free(base) );
    RET_VALUE = (u32) TRUE;
    RET_PARAMS(1);
}

uint Kernel32_FreeLibrary(Processor *cpu)
{
    V( cpu->Thr()->UnloadModule((HMODULE) PARAM(0)) );
    RET_VALUE = (u32) TRUE;
    RET_PARAMS(1);
}

uint Kernel32_GetACP(Processor *cpu)
{
    RET_VALUE = (u32) GetACP();
    RET_PARAMS(0);
}

uint Kernel32_GetAtomNameA(Processor *cpu)
{
	RET_VALUE = (u32) GetAtomNameA(
		(ATOM) PARAM(0),
		(LPSTR) PARAM_PTR(1),
		(int) PARAM(2)
		);
	RET_PARAMS(3);
}

uint Kernel32_GetCPInfo(Processor *cpu)
{
    LPCPINFO x = (LPCPINFO) PARAM_PTR(1);
    RET_VALUE = (u32) GetCPInfo(
        (UINT)      PARAM(0),
        (LPCPINFO)  PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_GetCommandLineA(Processor *cpu)
{
    const u32 BASE = 0x500000;
    static bool allocated = false;
    static u32 addr = 0;

    if (allocated) {
        cpu->EAX = addr;
        RET_PARAMS(0);
    } else {
        SyncObjectLock lock(*cpu->Mem);

        u32 b = cpu->Mem->FindFreePages(BASE, LX_CMDLINE_SIZE);
        Assert(b);
        V( cpu->Mem->AllocCopy(SectionDesc("CommandLineA", cpu->GetCurrentModule()), 
            b, LX_CMDLINE_SIZE, PAGE_READONLY, (pbyte) LxEmulator.CmdLine(), LX_CMDLINE_SIZE) );
        cpu->EAX = b;
        addr = b;
        allocated = true;
        RET_PARAMS(0);
    }
}

uint Kernel32_GetCommandLineW(Processor *cpu)
{
    const u32 BASE = 0x500000;

    static bool allocated = false;
    static u32 addr = 0;

    if (allocated) {
        cpu->EAX = addr;
        RET_PARAMS(0);
    } else {
        const char *path = LxEmulator.CmdLine();
        uint len = strlen(path);
        WCHAR pathW[MAX_PATH];
        LxByteToWide(path, pathW, len);
        uint lenW = (wcslen(pathW) + 1) * sizeof(wchar_t);

        SyncObjectLock lock(*cpu->Mem);
        u32 b = cpu->Mem->FindFreePages(BASE, lenW);
        Assert(b);
        V( cpu->Mem->AllocCopy(SectionDesc("CommandLineW", cpu->GetCurrentModule()), 
            b, lenW, PAGE_READONLY, (pbyte) pathW, lenW) );
        cpu->EAX = b;
        addr = b;
        allocated = true;
        RET_PARAMS(0);
    }
}

uint Kernel32_GetConsoleCP(Processor *cpu)
{
    RET_VALUE = (u32) GetConsoleCP();
    RET_PARAMS(0);
}

uint Kernel32_GetConsoleMode(Processor *cpu)
{
    RET_VALUE = (u32) GetConsoleMode(
        (HANDLE)    PARAM(0),
        (LPDWORD)   PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_GetCurrentDirectoryW(Processor *cpu)
{
	RET_VALUE = (u32) GetCurrentDirectoryW(
		(DWORD)		PARAM(0),
		(LPWSTR)	PARAM_PTR(1)
		);
	RET_PARAMS(2);
}

// uint Kernel32_GetCurrentPackageId(Processor *cpu)
// {
//     RET_VALUE = (u32) GetCurrentPackageId(
//         (UINT32 *)  PARAM_PTR(0),
//         (BYTE *)    PARAM_PTR(1)
//         );
//     RET_PARAMS(2);
// }

uint Kernel32_GetCurrentProcess(Processor *cpu)
{
    RET_VALUE = (u32) GetCurrentProcess();
    RET_PARAMS(0);
}

uint Kernel32_GetCurrentProcessId(Processor *cpu)
{
    RET_VALUE = (u32) GetCurrentProcessId();
    RET_PARAMS(0);
}

uint Kernel32_GetCurrentThreadId(Processor *cpu)
{
//    RET_VALUE = (u32) GetCurrentThreadId();
    RET_VALUE = (u32) cpu->Thr()->ID;
    RET_PARAMS(0);
}

uint Kernel32_GetCurrentThread(Processor *cpu)
{
    //RET_VALUE = (u32) GetCurrentThread();
    RET_VALUE = (u32) cpu->Thr()->Handle;
    RET_PARAMS(0);
}

uint Kernel32_GetDriveTypeA(Processor *cpu)
{
    cpu->EAX = (u32) GetDriveTypeA(
        (LPCSTR)    PARAM_PTR(0)
        );
    RET_PARAMS(1);
}
uint Kernel32_GetEnvironmentStrings(Processor *cpu)
{
#ifdef GetEnvironmentStrings
#undef GetEnvironmentStrings
#endif
    // TODO
    SyncObjectLock lock(*cpu->Mem);
    const u32 BASE = 0x500000;
    LPCSTR ptr = GetEnvironmentStrings();
    uint len = LxNNStrLen(ptr) + 1;
    u32 b = cpu->Mem->FindFreePages(BASE, len);
    Assert(b != 0);
    V( cpu->Mem->AllocCopy(SectionDesc("EnvStrA", cpu->GetCurrentModule()), 
        b, len, PAGE_READONLY, (pbyte) ptr, len) );
    cpu->EAX = b;
    return 0;
}

uint Kernel32_GetEnvironmentStringsW(Processor *cpu)
{
    // TODO
    SyncObjectLock lock(*cpu->Mem);
    const u32 BASE = 0x500000;
    LPCWSTR ptr = GetEnvironmentStringsW();
    uint len = LxNNStrLen(ptr) + sizeof(wchar_t);
    u32 b = cpu->Mem->FindFreePages(BASE, len);
    Assert(b != 0);
    V( cpu->Mem->AllocCopy(SectionDesc("EnvStrW", cpu->GetCurrentModule()), 
        b, len, PAGE_READONLY, (pbyte) ptr, len) );
    cpu->EAX = b;
    RET_PARAMS(0);
}

uint Kernel32_GetEnvironmentVariableA(Processor *cpu)
{
    RET_VALUE = (u32) GetEnvironmentVariableA(
        (LPCSTR)    PARAM_PTR(0),
        (LPSTR)     PARAM_PTR(1),
        (DWORD)     PARAM(2)
        );
    RET_PARAMS(3);
}

uint Kernel32_GetExitCodeProcess(Processor *cpu)
{
	RET_VALUE = (u32) GetExitCodeProcess(
		(HANDLE)	PARAM(0),
		(LPDWORD)	PARAM_PTR(1)
		);
	RET_PARAMS(2);
}

uint Kernel32_GetFileAttributesA(Processor *cpu)
{
	RET_VALUE = (u32) GetFileAttributesA((LPCSTR) PARAM_PTR(0));
	RET_PARAMS(1);
}

uint Kernel32_GetFileType(Processor *cpu)
{
    HANDLE hFile = (HANDLE) cpu->GetStackParam32(0);
    cpu->EAX = (u32) GetFileType(hFile);
    return 1;
}

uint Kernel32_GetLastError(Processor *cpu)
{
    cpu->EAX = (u32) GetLastError();
    return 0;
}

uint Kernel32_GetLocaleInfoW(Processor *cpu)
{
    RET_VALUE = (u32) GetLocaleInfoW(
        (LCID)      PARAM(0),
        (LCTYPE)    PARAM(1),
        (LPWSTR)    PARAM_PTR(2),
        (int)       PARAM(3)
        );
    RET_PARAMS(4);
}

uint Kernel32_GetLongPathNameA(Processor *cpu)
{
    RET_VALUE = (u32) GetLongPathNameA(
        (LPCSTR)    PARAM_PTR(0),
        (LPSTR)     PARAM_PTR(1),
        (DWORD)     PARAM(2)
        );
    return 3;
}

uint Kernel32_GetLongPathNameW(Processor *cpu)
{
    RET_VALUE = (u32) GetLongPathNameW(
        (LPCWSTR)   PARAM_PTR(0),
        (LPWSTR)    PARAM_PTR(1),
        (DWORD)     PARAM(2)
        );
    return 3;
}

uint Kernel32_GetModuleFileNameA(Processor *cpu)
{
    HMODULE hModule = (HMODULE) cpu->GetStackParam32(0);
    LPCH lpFilename = (LPCH) cpu->GetStackParamPtr32(1);
    DWORD nSize     = (DWORD) cpu->GetStackParam32(2);
    cpu->EAX        = cpu->Proc()->GetModuleFileName(hModule, lpFilename, nSize);
    RET_PARAMS(3);
}

uint Kernel32_GetModuleFileNameW(Processor *cpu)
{
    HMODULE hModule = (HMODULE) PARAM(0);
    LPWCH lpName = (LPWCH) PARAM_PTR(1);
    DWORD nSize = (DWORD) PARAM(2);
    RET_VALUE = cpu->Proc()->GetModuleFileName(hModule, lpName, nSize);
    RET_PARAMS(3);
}

uint Kernel32_GetModuleHandleA(Processor *cpu)
{
    LPCSTR lpName = (LPCSTR) PARAM_PTR(0);
    RET_VALUE = (u32) cpu->Proc()->GetModule(lpName);
    RET_PARAMS(1);
}

uint Kernel32_GetModuleHandleW(Processor *cpu)
{
    LPCWSTR lpName = (LPCWSTR) PARAM_PTR(0);
    RET_VALUE = (u32) cpu->Proc()->GetModule(lpName);
    RET_PARAMS(1);
}

uint Kernel32_GetModuleHandleExW(Processor *cpu)
{
    DWORD dwFlags = (DWORD) PARAM(0);
    if (dwFlags != 0) {
        LxFatal("GetModuleHandleEx() called with dwFlag != 0\n");
    }
    u32 h = (u32) cpu->Proc()->GetModule((LPCWSTR) PARAM_PTR(1));
    cpu->MemWrite32(PARAM(2), h);
    RET_VALUE = (h != 0);
    RET_PARAMS(3);
}

uint Kernel32_GetOEMCP(Processor *cpu)
{
    cpu->EAX = (u32) GetOEMCP();
    RET_PARAMS(0);
}
uint Kernel32_GetNumberOfConsoleInputEvents(Processor *cpu)
{
    RET_VALUE = (u32) GetNumberOfConsoleInputEvents(
        (HANDLE)        PARAM(0),
        (LPDWORD)       PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_GetProcAddress(Processor *cpu)
{
    HMODULE hModule = (HMODULE) PARAM(0);
    LPCSTR lpStr = (LPCSTR) PARAM_PTR(1);

    RET_VALUE = cpu->Proc()->GetProcAddr(hModule, lpStr);
    RET_PARAMS(2);
    //LxWarning("GetProcAddress: %s\n", lpStr);
}

uint Kernel32_GetProcessHeap(Processor *cpu)
{
    Assert(LxEmulator.Proc()->GetHeap(Process::ProcessHeapStart));
    RET_VALUE = Process::ProcessHeapStart; /* process heap handle is 0 */
    RET_PARAMS(0);
}

uint Kernel32_GetStartupInfoA(Processor *cpu)
{
    LPSTARTUPINFOA lpInfo = (LPSTARTUPINFOA) cpu->GetStackParamPtr32(0);
    GetStartupInfoA(lpInfo);
    LxWarning("Kernel32.dll::GetStartupInfoA() needs refinement\n");
    return 1;
}

uint Kernel32_GetStartupInfoW(Processor *cpu)
{
    GetStartupInfoW(
        (LPSTARTUPINFOW)    PARAM_PTR(0)
        );
    LxWarning("Kernel32.dll::GetStartupInfow() needs refinement\n");
    RET_PARAMS(1);
}

uint Kernel32_GetStdHandle(Processor *cpu)
{
    u32 val = PARAM(0);
    if (LxConfig.GetInt("Winapi", "OverrideStdHandles", 0)) {
        if (val == STD_INPUT_HANDLE)
            cpu->EAX = LxConfig.GetInt("Winapi", "HasStdInputHandle", 0) ?
            (u32) GetStdHandle(STD_INPUT_HANDLE) : 0;
        else if (val == STD_OUTPUT_HANDLE)
            cpu->EAX = LxConfig.GetInt("Winapi", "HasStdOutputHandle", 0) ?
            (u32) GetStdHandle(STD_OUTPUT_HANDLE) : 0;
        else if (val == STD_ERROR_HANDLE)
            cpu->EAX = LxConfig.GetInt("Winapi", "HasStdErrorHandle", 0) ? 
            (u32) GetStdHandle(STD_ERROR_HANDLE) : 0;
        else
            cpu->EAX = (u32) GetStdHandle(val);
    } else {
        cpu->EAX = (u32) GetStdHandle(val);
    }
    return 1;
}

uint Kernel32_GetStringTypeA(Processor *cpu)
{
    RET_VALUE = (u32) GetStringTypeA(
        (LCID)      PARAM(0),
        (DWORD)     PARAM(1),
        (LPCSTR)    PARAM_PTR(2),
        (int)       PARAM(3),
        (LPWORD)    PARAM_PTR(4)
        );
    RET_PARAMS(5);
}

uint Kernel32_GetStringTypeW(Processor *cpu)
{
    RET_VALUE = (u32) GetStringTypeW(
        (DWORD)     PARAM(0),
        (LPCWSTR)   PARAM_PTR(1),
        (int)       PARAM(2),
        (LPWORD)    PARAM_PTR(3)
        );
    RET_PARAMS(4);
}

uint Kernel32_GetSystemTimeAsFileTime(Processor *cpu)
{
    GetSystemTimeAsFileTime(
        (LPFILETIME)    PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_GetTempPathA(Processor *cpu)
{
    RET_VALUE = (u32) GetTempPathA(
        (DWORD)     PARAM(0),
        (LPSTR)     PARAM_PTR(1)
        );
    RET_PARAMS(2);

}

uint Kernel32_GetThreadTimes(Processor *cpu)
{
    RET_VALUE = (u32) GetThreadTimes(
        (HANDLE)        PARAM(0),
        (LPFILETIME)    PARAM_PTR(1),
        (LPFILETIME)    PARAM_PTR(2),
        (LPFILETIME)    PARAM_PTR(3),
        (LPFILETIME)    PARAM_PTR(4)
        );
    RET_PARAMS(5);
}

uint Kernel32_GetTickCount(Processor *cpu)
{
    RET_VALUE = (u32) GetTickCount();
    RET_PARAMS(0);
}

uint Kernel32_GetTickCount64(Processor *cpu)
{
    RET_VALUE = (u32) GetTickCount64();
    RET_PARAMS(0);
}

uint Kernel32_GetTimeZoneInformation(Processor *cpu)
{
	RET_VALUE = (u32) GetTimeZoneInformation(
		(LPTIME_ZONE_INFORMATION)	PARAM_PTR(0)
		);
	RET_PARAMS(1);
}

uint Kernel32_GetUserDefaultLCID(Processor *cpu)
{
    RET_VALUE = (u32) GetUserDefaultLCID();
    RET_PARAMS(0);
}

uint Kernel32_GetVersion( Processor *cpu )
{
    cpu->EAX = (u32) GetVersion();
    return 0;
}

uint Kernel32_GetVersionExA(Processor *cpu)
{
    cpu->EAX = (u32) GetVersionExA(
        (LPOSVERSIONINFOA) PARAM_PTR(0)
        );
    RET_PARAMS(1);
}
uint Kernel32_GetVersionExW(Processor *cpu)
{
    RET_VALUE = (u32) GetVersionExW(
        (LPOSVERSIONINFOW)  PARAM_PTR(0)
        );
    RET_PARAMS(1);
}
uint Kernel32_GetVolumeInformationA(Processor *cpu)
{
    RET_VALUE = (u32) GetVolumeInformationA(
        (LPCSTR)    PARAM_PTR(0),
        (LPSTR)     PARAM_PTR(1),
        (DWORD)     PARAM(2),
        (LPDWORD)   PARAM_PTR(3),
        (LPDWORD)   PARAM_PTR(4),
        (LPDWORD)   PARAM_PTR(5),
        (LPSTR)     PARAM_PTR(6),
        (DWORD)     PARAM(7)
        );
    return 8;
}

uint Kernel32_GlobalAlloc(Processor *cpu)
{
	LxWarning("Kernel32::GlobalAlloc()\n");

    SyncObjectLock lock(*cpu->Mem);

	Heap *heap = LxEmulator.Proc()->GetHeap(Process::ProcessHeapStart);
	Assert(heap);
	DWORD dwFlag = (DWORD) PARAM(0);
	SIZE_T dwBytes = (SIZE_T) PARAM(1);
	
	RET_VALUE = (u32) heap->HeapAlloc(dwBytes, dwFlag, cpu);
	RET_PARAMS(2);
}

uint Kernel32_GlobalFree(Processor *cpu)
{
	LxWarning("Kernel32::GlobalFree()\n");

    SyncObjectLock lock(*cpu->Mem);

	HGLOBAL hg = (HGLOBAL) PARAM(0);
	Heap *heap = LxEmulator.Proc()->GetHeap(Process::ProcessHeapStart);
	Assert(heap);
	RET_VALUE = (u32) (heap->HeapFree((u32)hg, 0, cpu) ? NULL : hg);
	RET_PARAMS(1);
}

uint Kernel32_GlobalMemoryStatus(Processor *cpu)
{
    LPMEMORYSTATUS lpMem = (LPMEMORYSTATUS) cpu->GetStackParamPtr32(0);
    GlobalMemoryStatus(
        (LPMEMORYSTATUS)    PARAM_PTR(0)
        );
    return 1;
}

uint Kernel32_HeapAlloc( Processor *cpu )
{
    SyncObjectLock lock(*cpu->Mem);

    HANDLE hHeap = (HANDLE) cpu->GetStackParam32(0);
    DWORD dwFlag = (DWORD) cpu->GetStackParam32(1);
    SIZE_T dwBytes = (SIZE_T) cpu->GetStackParam32(2);
    Heap *h = LxEmulator.Proc()->GetHeap((uint) hHeap);
    Assert(h);
    cpu->EAX = (u32) h->HeapAlloc(dwBytes, dwFlag, cpu);
    return 3;
}

uint Kernel32_HeapCreate(Processor *cpu)
{
    u32 flags = PARAM(0);
    u32 initSize = PARAM(1);
    u32 maxSize = PARAM(2);

    SyncObjectLock lock(*cpu->Mem);

    if (maxSize == 0) maxSize = 0x10000000;
    HeapID heapId = LxEmulator.Proc()->CreateHeap(maxSize, initSize, flags);
    RET_VALUE = (u32) heapId;
    RET_PARAMS(3);
}

uint Kernel32_HeapDestroy(Processor *cpu)
{
    HANDLE hHeap = (HANDLE) PARAM(0);

    SyncObjectLock lock(*cpu->Mem);

    RET_VALUE = (u32) LxEmulator.Proc()->DestroyHeap((HeapID) hHeap);
    RET_PARAMS(1);
}

uint Kernel32_HeapFree( Processor *cpu )
{
    HANDLE  hHeap   = (HANDLE) PARAM(0);
    DWORD   dwFlag  = (DWORD)  PARAM(1);
    LPVOID  lpMem   = (LPVOID) PARAM(2);

    SyncObjectLock lock(*cpu->Mem);

    Heap *h = LxEmulator.Proc()->GetHeap((u32) hHeap);
    Assert(h);
    cpu->EAX = (u32) h->HeapFree((u32) lpMem, dwFlag, cpu);
    return 3;
}

uint Kernel32_HeapReAlloc(Processor *cpu)
{
    u32 hHeap = PARAM(0);
    uint dwFlag = PARAM(1);
    u32 lpMem = PARAM(2);
    u32 size = PARAM(3);

    SyncObjectLock lock(*cpu->Mem);

    Heap *h = LxEmulator.Proc()->GetHeap(hHeap);
    Assert(h);
    RET_VALUE = h->HeapRealloc(lpMem, size, dwFlag, cpu);
    RET_PARAMS(4);
}

uint Kernel32_HeapValidate(Processor *cpu)
{
    SyncObjectLock lock(*cpu->Mem);

	RET_VALUE = (u32) HeapValidate(
		(HANDLE)PARAM(0), (DWORD)PARAM(1), (LPCVOID)PARAM_PTR(2));
	RET_PARAMS(3);
}

uint Kernel32_HeapSetInformation(Processor *cpu)
{
	RET_VALUE = (u32) HeapSetInformation(
		(HANDLE)	PARAM(0),
		(HEAP_INFORMATION_CLASS) PARAM(1),
		(PVOID)		PARAM_PTR(2),
		(SIZE_T)	PARAM(3)
		);
    LxWarning("HeapSetInformation may contain error\n");
	RET_PARAMS(4);
}

uint Kernel32_HeapSize(Processor *cpu)
{
    HANDLE hHeap = (HANDLE) PARAM(0);
    DWORD dwFlag = (DWORD) PARAM(1);
    LPCVOID lpMem = (LPCVOID) PARAM(2);
    Heap *h = LxEmulator.Proc()->GetHeap((u32) hHeap);
    Assert(h);
    RET_VALUE = h->HeapSize((u32) lpMem, dwFlag, cpu);
    RET_PARAMS(3);
}

uint Kernel32_InitializeCriticalSection(Processor *cpu)
{
	InitializeCriticalSection(
		(LPCRITICAL_SECTION)	PARAM_PTR(0)
		);
	RET_PARAMS(1);
}

uint Kernel32_InitializeCriticalSectionAndSpinCount( Processor *cpu )
{
    LPCRITICAL_SECTION x = (LPCRITICAL_SECTION) PARAM_PTR(0);
    RET_VALUE = (u32) InitializeCriticalSectionAndSpinCount(
        (LPCRITICAL_SECTION)    PARAM_PTR(0),
        (DWORD)                 PARAM(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_InitializeCriticalSectionEx(Processor *cpu)
{
    RET_VALUE = (u32) InitializeCriticalSectionEx(
        (LPCRITICAL_SECTION)    PARAM_PTR(0),
        (DWORD)                 PARAM(1),
        (DWORD)                 PARAM(2)
        );
    RET_PARAMS(3);
}

uint Kernel32_InitializeSListHead(Processor *cpu)
{
    InitializeSListHead(
        (PSLIST_HEADER)     PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_InitOnceExecuteOnce(Processor *cpu)
{
    PINIT_ONCE_FN p     = (PINIT_ONCE_FN) PARAM(1);
    PINIT_ONCE InitOnce = (PINIT_ONCE) PARAM_PTR(0);
    PVOID Parameter     = (PVOID) PARAM_PTR(2);
    LPVOID *Context     = (LPVOID *)    PARAM_PTR(3);

    cpu->SetCallbackEntry(LX_CALLBACK_INITONCE, (u32) p);
    RET_VALUE = (u32) InitOnceExecuteOnce(
        (PINIT_ONCE)        PARAM_PTR(0),
        LxInitOnceCallback,
        (PVOID)             PARAM_PTR(2),
        (LPVOID *)          PARAM_PTR(3)
        );
    RET_PARAMS(4);
}

uint Kernel32_InterlockedCompareExchange(Processor *cpu)
{
    RET_VALUE = (u32) InterlockedCompareExchange(
        (volatile LONG *)   PARAM_PTR(0),
        (LONG)              PARAM(1),
        (LONG)              PARAM(2)
        );
    RET_PARAMS(3);
}

uint Kernel32_InterlockedDecrement(Processor *cpu)
{
    RET_VALUE = (u32) InterlockedDecrement(
        (LONG volatile *)   PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_InterlockedExchange(Processor *cpu)
{
    RET_VALUE = (u32) InterlockedExchange(
        (volatile LONG *)   PARAM_PTR(0),
        (LONG)              PARAM(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_InterlockedIncrement(Processor *cpu)
{

    RET_VALUE = (u32) InterlockedIncrement(
        (LONG volatile *)   PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_IsBadReadPtr(Processor *cpu)
{
	RET_VALUE = (u32) IsBadReadPtr(
		(const void *)PARAM_PTR(0), (UINT_PTR)PARAM(1));
	RET_PARAMS(2);
}

uint Kernel32_IsBadWritePtr(Processor *cpu)
{
	RET_VALUE = (u32) IsBadWritePtr(
		(LPVOID)PARAM_PTR(0), (UINT_PTR)PARAM(1));
	RET_PARAMS(2);
}

uint Kernel32_IsDBCSLeadByte(Processor *cpu)
{
    BYTE b = (u8) cpu->GetStackParam32(0);
    cpu->EAX = (u32) IsDBCSLeadByte(b);
    return 1;
}

uint Kernel32_IsDebuggerPresent(Processor *cpu)
{
    // TODO
    RET_VALUE = (u32) 0;
    RET_PARAMS(0);
}

uint Kernel32_IsProcessorFeaturePresent(Processor *cpu)
{
    RET_VALUE = (u32) IsProcessorFeaturePresent(
        (DWORD)     PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_IsValidCodePage(Processor *cpu)
{
    RET_VALUE = (u32) IsValidCodePage(
        (UINT)      PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_IsValidLocale(Processor *cpu)
{
    RET_VALUE = (u32) IsValidLocale(
        (LCID)      PARAM(0),
        (DWORD)     PARAM(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_LCMapStringA(Processor *cpu)
{
    RET_VALUE = (u32) LCMapStringA(
        (LCID)      PARAM(0),
        (DWORD)     PARAM(1),
        (LPCSTR)   PARAM_PTR(2),
        (int)       PARAM(3),
        (LPSTR)    PARAM_PTR(4),
        (int)       PARAM(5)
        );
    RET_PARAMS(6);
}

uint Kernel32_LCMapStringEx(Processor *cpu)
{
    RET_VALUE = (u32) LCMapStringEx(
        (LPCWSTR)   PARAM_PTR(0),
        (DWORD)     PARAM(1),
        (LPCWSTR)   PARAM_PTR(2),
        (int)       PARAM(3),
        (LPWSTR)    PARAM_PTR(4),
        (int)       PARAM(5),
        (LPNLSVERSIONINFO)  PARAM_PTR(6),
        (LPVOID)    PARAM_PTR(7),
        (LPARAM)    PARAM(8)
        );
    RET_PARAMS(9);
}

uint Kernel32_LCMapStringW(Processor *cpu)
{
    RET_VALUE = (u32) LCMapStringW(
        (LCID)      PARAM(0),
        (DWORD)     PARAM(1),
        (LPCWSTR)   PARAM_PTR(2),
        (int)       PARAM(3),
        (LPWSTR)    PARAM_PTR(4),
        (int)       PARAM(5)
        );
    RET_PARAMS(6);
}

uint Kernel32_LeaveCriticalSection(Processor *cpu)
{
    LeaveCriticalSection(
        (LPCRITICAL_SECTION)    PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_LoadLibraryA(Processor *cpu)
{
     LPCSTR lpName = (LPCSTR) PARAM_PTR(0);
 
     if (IsEmulatedLibrary(lpName)) {
         RET_VALUE = (u32) LxGetModuleHandle(lpName);
     } else {
         RET_VALUE = (u32) cpu->Thr()->LoadModule(lpName);
     }

    RET_PARAMS(1);
}

uint Kernel32_LoadLibraryW(Processor *cpu)
{
	LPCWSTR lpName = (LPCWSTR) PARAM_PTR(0);
	char buf[128];
	int len = wcslen(lpName);
	LxWideToByte(lpName, buf, len);
 	if(IsEmulatedLibrary(buf)) {
 		RET_VALUE = (u32) LxGetModuleHandle(buf);
 	} else {
 		RET_VALUE = (u32) cpu->Thr()->LoadModule(buf);
 	}
	RET_PARAMS(1);
}

uint Kernel32_LoadLibraryExW(Processor *cpu)
{
    LPCWSTR lpName = (LPCWSTR) PARAM_PTR(0);
    char buf[128];
    int len = wcslen(lpName);
    LxWideToByte(lpName, buf, len);
    LxWarning("Kernel32::LoadLibraryExW(), loading %s\n", buf);
    RET_VALUE = 0;      // default fails
    RET_PARAMS(3);
}

uint Kernel32_lstrcpy(Processor *cpu)
{
    RET_VALUE = (u32) lstrcpyA(
        (LPSTR)     PARAM_PTR(0),
        (LPCSTR)    PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_lstrcmpiA(Processor *cpu)
{
    RET_VALUE = (u32) lstrcmpiA(
        (LPCSTR)        PARAM_PTR(0),
        (LPCSTR)        PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_lstrlenA(Processor *cpu)
{
    RET_VALUE = (u32) lstrlenA(
        (LPCSTR)        PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_MultiByteToWideChar(Processor *cpu)
{
    RET_VALUE = (u32) MultiByteToWideChar(
        (UINT)      PARAM(0),
        (DWORD)     PARAM(1),
        (LPCSTR)    PARAM_PTR(2),
        (int)       PARAM(3),
        (LPWSTR)    PARAM_PTR(4),
        (int)       PARAM(5)
        );
    RET_PARAMS(6);
}

uint Kernel32_PeekConsoleInputA(Processor *cpu)
{
    RET_VALUE = (u32) PeekConsoleInputA(
        (HANDLE)        PARAM(0),
        (PINPUT_RECORD) PARAM_PTR(1),
        (DWORD)         PARAM(2),
        (LPDWORD)       PARAM_PTR(3)
        );
    RET_PARAMS(4);
}

uint Kernel32_QueryPerformanceCounter(Processor *cpu)
{
    RET_VALUE = (u32) QueryPerformanceCounter(
        (LARGE_INTEGER *)   PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_RaiseException(Processor *cpu)
{
    NOT_IMPLEMENTED();
    return 0;
}

uint Kernel32_ReadConsoleInputA(Processor *cpu)
{
    RET_VALUE = (u32) ReadConsoleInputA(
        (HANDLE)        PARAM(0),
        (PINPUT_RECORD) PARAM_PTR(1),
        (DWORD)         PARAM(2),
        (LPDWORD)       PARAM_PTR(3)
        );
    RET_PARAMS(4);
}

uint Kernel32_ReadFile(Processor *cpu)
{
    HANDLE hFile = (HANDLE) cpu->GetStackParam32(0);
    LPVOID lpBuffer = (LPVOID) cpu->GetStackParamPtr32(1);
    DWORD nNumToRead = (DWORD) cpu->GetStackParam32(2);
    LPDWORD nNumRead = (LPDWORD) cpu->GetStackParamPtr32(3);
    LPOVERLAPPED lpOverlapped = (LPOVERLAPPED) cpu->GetStackParamPtr32(4);
    cpu->EAX = (u32) ReadFile(hFile, lpBuffer, nNumToRead, nNumRead, lpOverlapped);
    return 5;
}

uint Kernel32_RtlUnwind(Processor *cpu)
{
    //NOT_IMPLEMENTED();
    LxWarning("Kernel32::RtlUnwind() called\n");
//     RtlUnwind(
//         (PVOID)     PARAM_PTR(0),
//         (PVOID)     PARAM_PTR(1),
//         (PEXCEPTION_RECORD)   PARAM_PTR(2),
//         (PVOID)     PARAM_PTR(3)
//         );
    RET_PARAMS(4);
}

uint Kernel32_SearchPathA(Processor *cpu)
{
    u32 bufferAddr  = (u32) PARAM(4);
    LPSTR   lpBuffer = (LPSTR) PARAM_PTR(4);
    LPSTR * lpPart = (LPSTR *)  PARAM_PTR(5);

    RET_VALUE = (u32) SearchPathA(
        (LPCSTR)        PARAM_PTR(0),
        (LPCSTR)        PARAM_PTR(1),
        (LPCSTR)        PARAM_PTR(2),
        (DWORD)         PARAM(3),
        lpBuffer,
        lpPart
        );

    if (lpPart != NULL && *lpPart != NULL) {
        LxFatal("take care of this\n");
        *lpPart = (LPSTR) (bufferAddr + (*lpPart - lpBuffer));
    }

    RET_PARAMS(6);
}

uint Kernel32_SetFileAttributesA(Processor *cpu)
{
    RET_VALUE=(u32) SetFileAttributesA(
        (LPCSTR)PARAM_PTR(0),
        (DWORD) PARAM(1)
        );
    RET_PARAMS(2);
}

uint Kernel32_SetConsoleCtrlHandler(Processor *cpu)
{
    PHANDLER_ROUTINE p = (PHANDLER_ROUTINE) cpu->GetStackParam32(0);
    cpu->SetCallbackEntry(LX_CALLBACK_CONSOLE_CTRL_HANDLER, (u32) p);
    BOOL b = (BOOL) cpu->GetStackParam32(1);
    cpu->EAX = (u32) SetConsoleCtrlHandler(LxHandlerRoutine, b);
    return 2;
}

uint Kernel32_SetConsoleMode(Processor *cpu)
{
    HANDLE hConsole = (HANDLE) cpu->GetStackParam32(0);
    DWORD dwMode = (DWORD) cpu->GetStackParam32(1);
    cpu->EAX = (u32) SetConsoleMode(hConsole, dwMode);
    return 2;
}

uint Kernel32_SetConsoleTitleA(Processor *cpu)
{
    RET_VALUE = (u32) SetConsoleTitleA(
        (LPCSTR)        PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_SetErrorMode(Processor *cpu)
{
	RET_VALUE = (u32) SetErrorMode(
		(UINT)	PARAM(0)
		);
	RET_PARAMS(1);
}

uint Kernel32_SetFileApisToOEM(Processor *cpu)
{
	SetFileApisToOEM();
	return 0;
}

uint Kernel32_SetFilePointer(Processor *cpu)
{
    HANDLE hFile = (HANDLE) cpu->GetStackParam32(0);
    LONG lDist = (LONG) cpu->GetStackParam32(1);
    PLONG pDistHigh = (PLONG) cpu->GetStackParamPtr32(2);
    DWORD dwFlag = (DWORD) cpu->GetStackParam32(3);
    cpu->EAX = (u32) SetFilePointer(hFile, lDist, pDistHigh, dwFlag);
    return 4;
}

uint Kernel32_SetHandleCount(Processor *cpu)
{
    UINT n = (UINT) cpu->GetStackParam32(0);
    cpu->EAX = (u32) SetHandleCount(n);
    return 1;
}

uint Kernel32_SetLastError(Processor *cpu)
{
    SetLastError(
        (DWORD) PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_SetUnhandledExceptionFilter(Processor *cpu)
{
//     LPTOP_LEVEL_EXCEPTION_FILTER p = (LPTOP_LEVEL_EXCEPTION_FILTER) PARAM(0);
//     cpu->SetCallbackEntry(LX_CALLBACK_TOPLEVEL_EXCEPTION_FILTER, (u32) p);
//     RET_VALUE = (u32) SetUnhandledExceptionFilter(
//         LxTopLevelExceptionFilter
//         );
    LxError("SetUnhandledExceptionFilter()\n");
    RET_VALUE = 0;
    RET_PARAMS(1);
}

uint Kernel32_Sleep(Processor *cpu)
{
    Sleep(
        (DWORD) PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_TerminateProcess(Processor *cpu)
{
    HANDLE hCurrProc = GetCurrentProcess();
    HANDLE hProc = (HANDLE) PARAM(0);
    UINT nExit = (UINT) PARAM(1);
    if (hCurrProc == hProc) {
        cpu->Terminate(nExit);
    } else {
        RET_VALUE = (u32) TerminateProcess(
            hProc, nExit
            );
    }
    RET_PARAMS(2);
}

uint Kernel32_TlsAlloc(Processor *cpu)
{
    RET_VALUE = (u32) TlsAlloc();
    RET_PARAMS(0);
}

uint Kernel32_TlsFree(Processor *cpu)
{
    RET_VALUE = (u32) TlsFree(
        (DWORD)     PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_TlsGetValue(Processor *cpu)
{
    RET_VALUE = (u32) TlsGetValue(
        (DWORD)     PARAM(0)
        );
    RET_PARAMS(1);
}

uint Kernel32_TlsSetValue(Processor *cpu)
{
    RET_VALUE = (u32) TlsSetValue(
        (DWORD)     PARAM(0),
        (LPVOID)    PARAM(1)    /* This parameter is just a value */
        );
    RET_PARAMS(2);
}

uint Kernel32_UnhandledExceptionFilter(Processor *cpu)
{
//     struct _EXCEPTION_POINTERS *p = (struct _EXCEPTION_POINTERS *)PARAM_PTR(0);
//     p->ExceptionRecord = (PEXCEPTION_RECORD) cpu->MemRead32((u32) p->ExceptionRecord);
//     p->ContextRecord = (PCONTEXT) cpu->MemRead32((u32) p->ContextRecord);
// 
//     RET_VALUE = (u32) UnhandledExceptionFilter(
//         (struct _EXCEPTION_POINTERS *)  PARAM_PTR(0)
//         );
    LxError("UnhandledExceptionFilter not implemented\n");
    RET_VALUE = EXCEPTION_EXECUTE_HANDLER;
    RET_PARAMS(1);
}

uint Kernel32_VirtualAlloc(Processor *cpu)
{
    LPVOID lpAddress    = (LPVOID) cpu->GetStackParam32(0); // Don't need pointer convertion
    SIZE_T s            = (SIZE_T) cpu->GetStackParam32(1);
    DWORD dwType        = (DWORD) cpu->GetStackParam32(2);
    DWORD dwProtect     = (DWORD) cpu->GetStackParam32(3);

    LxResult lr;
    u32 result;

    SyncObjectLock lock(*cpu->Mem);

    switch (dwType) {
        case MEM_RESERVE:
            {
                if (lpAddress == NULL) {
                    lpAddress = (LPVOID) LxEmulator.Mem()->FindFreePages(0x2000000, (u32) s);
                } else {
                    lpAddress = (LPVOID) (((u32) lpAddress) & PAGE_ALLOC_MASK);
                }
                Assert( (((u32) lpAddress) & ~PAGE_ALLOC_MASK) == 0 );
                lr = LxEmulator.Mem()->Reserve(SectionDesc("v-alloc", cpu->GetCurrentModule()), 
                    (u32) lpAddress, (u32) s, dwProtect);
                result = LX_FAILED(lr) ? 0 : (u32) lpAddress;
            } break;
        case MEM_COMMIT:
            {
                lr = LxEmulator.Mem()->Commit((u32) lpAddress, (u32) s, dwProtect);
                result = LX_FAILED(lr) ? 0 : (u32) lpAddress;
            } break;
        case MEM_RESERVE | MEM_COMMIT:
            {
                if (lpAddress == NULL) {
                    lpAddress = (LPVOID) LxEmulator.Mem()->FindFreePages(0x2000000, (u32) s);
                } else {
                    lpAddress = (LPVOID) (((u32) lpAddress) & PAGE_ALLOC_MASK);
                }
                Assert( (((u32) lpAddress) & ~PAGE_ALLOC_MASK) == 0 );
                lr = LxEmulator.Mem()->Alloc(SectionDesc("v-alloc", cpu->GetCurrentModule()), 
                    (u32) lpAddress, (u32) s, dwProtect);
                result = LX_FAILED(lr) ? 0 : (u32) lpAddress;
            } break;
        default:
            NOT_IMPLEMENTED();
    }
    
    cpu->EAX = result;
    return 4;
}

uint Kernel32_VirtualFree(Processor *cpu)
{
    LPVOID lpAddress = (LPVOID) cpu->GetStackParam32(0); // Don't need pointer convertion here
    DWORD dwSize = (DWORD) cpu->GetStackParam32(1);
    DWORD dwType = (DWORD) cpu->GetStackParam32(2);
    
    SyncObjectLock lock(*cpu->Mem);

    LxResult lr;
    switch (dwType) {
        case MEM_DECOMMIT:
            {
                lr = LxEmulator.Mem()->Decommit((u32) lpAddress, dwSize);
            } break;
        case MEM_RELEASE:
            {
                if (dwSize != 0) { lr = LX_RESULT_INVALID_OPERATION; break; }
                lr = LxEmulator.Mem()->Release((u32) lpAddress);
            } break;
        case MEM_RELEASE | MEM_DECOMMIT:
            {
                if (dwSize != 0) { lr = LX_RESULT_INVALID_OPERATION; break; }
                lr = LxEmulator.Mem()->Free((u32) lpAddress);
            } break;
        default:
            NOT_IMPLEMENTED();
    }

    cpu->EAX = LX_FAILED(lr) ? 0 : TRUE;
    return 3;
}

uint Kernel32_WaitForSingleObject(Processor *cpu)
{
    HANDLE hObj = (HANDLE) PARAM(0);

	RET_VALUE = (u32) WaitForSingleObject(
		hObj,
		(DWORD)		PARAM(1)
		);
	RET_PARAMS(2);
}

uint Kernel32_WideCharToMultiByte(Processor *cpu)
{
    RET_VALUE = (u32) WideCharToMultiByte(
        (UINT)      PARAM(0),
        (DWORD)     PARAM(1),
        (LPCWSTR)   PARAM_PTR(2),
        (int)       PARAM(3),
        (LPSTR)     PARAM_PTR(4),
        (int)       PARAM(5),
        (LPCSTR)    PARAM_PTR(6),
        (LPBOOL)    PARAM_PTR(7)
        );
    RET_PARAMS(8);
}

uint Kernel32_WinExec(Processor *cpu)
{
    RET_VALUE = WinExec(
        (LPCSTR)    PARAM_PTR(0),
        (UINT)      PARAM(1)
        );
    RET_PARAMS(2);
}
uint Kernel32_WriteFile(Processor *cpu)
{
    HANDLE hFile = (HANDLE) cpu->GetStackParam32(0);
    LPVOID lpBuffer = (LPVOID) cpu->GetStackParamPtr32(1);
    DWORD nNumToWrite = (DWORD) cpu->GetStackParam32(2);
    LPDWORD nNumWritten = (LPDWORD) cpu->GetStackParamPtr32(3);
    LPOVERLAPPED lpOverlapped = (LPOVERLAPPED) cpu->GetStackParamPtr32(4);
    cpu->EAX = (u32) WriteFile(hFile, lpBuffer, nNumToWrite, nNumWritten, lpOverlapped);
    return 5;
}

END_NAMESPACE_LOCHSEMU()