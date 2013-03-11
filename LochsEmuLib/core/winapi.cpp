#include "stdafx.h"
#include "winapi.h"
#include "processor.h"
#include "debug.h"

BEGIN_NAMESPACE_LOCHSEMU()

const char *WrappedLibraryTable[] = {
    /*   0 */ "",
    /*   1 */ "kernel32.dll",
    /*   2 */ "user32.dll",
    /*   3 */ "gdi32.dll",
    /*   4 */ "PGORT90.dll",
    /*   5 */ "winmm.dll",
    /*   6 */ "kernelbase.dll",
    /*   7 */ "ntdll.dll",
	/*   8 */ "advapi32.dll",
    /*   9 */ "sechost.dll",
    /*  10 */ "rpcrt4.dll",
    /*  11 */ "sspicli.dll",
    /*  12 */ "cryptbase.dll",
    /*  13 */ "lpk.dll",
    /*  14 */ "usp10.dll",
    /*  15 */ "comctl32.dll",
    /*  16 */ "ws2_32.dll",
    /*  17 */ "shlwapi.dll",
    /*  18 */ "shell32.dll",
	/*  19 */ "oleaut32.dll",
};

WinAPIInfo WinAPIInfoTable[] = {
    { 00, 0, "", WinAPINotAvailable },
    
    /* kernel32.dll */
	{ 01, 0, "AddAtomA", Kernel32_AddAtomA },
    { 01, 0, "AreFileApisANSI", Kernel32_AreFileApisANSI },
    { 01, 0, "CloseHandle", Kernel32_CloseHandle },
	{ 01, 0, "CompareStringW", Kernel32_CompareStringW },
    { 01, 0, "CreateFileA", Kernel32_CreateFileA },
    { 01, 0, "CreateFileW", Kernel32_CreateFileW },
	{ 01, 0, "CreateProcessA", kernel32_CreateProcessA },
	{ 01, 0, "CreateSemaphoreA", Kernel32_CreateSemaphoreA },
    { 01, 0, "DecodePointer", Kernel32_DecodePointer },
    { 01, 0, "DeleteCriticalSection", Kernel32_DeleteCriticalSection },
    { 01, 0, "DeleteFileA", Kernel32_DeleteFileA },
	{ 01, 0, "DisableThreadLibraryCalls", Kernel32_DisableThreadLibraryCalls },
    { 01, 0, "EncodePointer", Kernel32_EncodePointer },
    { 01, 0, "EnterCriticalSection", Kernel32_EnterCriticalSection },
    { 01, 0, "ExitProcess", Kernel32_ExitProcess },
    { 01, 0, "FindActCtxSectionStringW", Kernel32_FindActCtxSectionStringW },
	{ 01, 0, "FindAtomA", Kernel32_FindAtomA },
	{ 01, 0, "FindClose", Kernel32_FindClose },
	{ 01, 0, "FindFirstFileW", Kernel32_FindFirstFileW },
	{ 01, 0, "FindResourceExA", Kernel32_FindResourceExA },
    { 01, 0, "FlsAlloc", Kernel32_FlsAlloc },
    { 01, 0, "FlsFree", Kernel32_FlsFree },
    { 01, 0, "FlsGetValue", Kernel32_FlsGetValue },
    { 01, 0, "FlsSetValue", Kernel32_FlsSetValue },
    { 01, 0, "FlushConsoleInputBuffer", Kernel32_FlushConsoleInputBuffer },
    { 01, 0, "FreeEnvironmentStringsW", Kernel32_FreeEnvironmentStringsW },
    { 01, 0, "GetACP", Kernel32_GetACP },
	{ 01, 0, "GetAtomNameA", Kernel32_GetAtomNameA },
    { 01, 0, "GetCPInfo", Kernel32_GetCPInfo },
    { 01, 0, "GetConsoleMode", Kernel32_GetConsoleMode },
    { 01, 0, "GetCommandLineA", Kernel32_GetCommandLineA },
    { 01, 0, "GetCommandLineW", Kernel32_GetCommandLineW },
	{ 01, 0, "GetCurrentDirectoryW", Kernel32_GetCurrentDirectoryW },
    //{ 01, 0, "GetCurrentPackageId", Kernel32_GetCurrentPackageId },
    { 01, 0, "GetCurrentProcess", Kernel32_GetCurrentProcess },
    { 01, 0, "GetCurrentProcessId", Kernel32_GetCurrentProcessId },
    { 01, 0, "GetCurrentThreadId", Kernel32_GetCurrentThreadId },
    { 01, 0, "GetCurrentThread", Kernel32_GetCurrentThread },
    { 01, 0, "GetDriveTypeA", Kernel32_GetDriveTypeA },
    { 01, 0, "GetEnvironmentStrings", Kernel32_GetEnvironmentStrings },
    { 01, 0, "GetEnvironmentStringsW", Kernel32_GetEnvironmentStringsW },
    { 01, 0, "GetEnvironmentVariableA", Kernel32_GetEnvironmentVariableA },
	{ 01, 0, "GetExitCodeProcess", Kernel32_GetExitCodeProcess },
	{ 01, 0, "GetFileAttributesA", Kernel32_GetFileAttributesA },
    { 01, 0, "GetFileType", Kernel32_GetFileType },
    { 01, 0, "GetLastError", Kernel32_GetLastError },
    { 01, 0, "GetLongPathNameA", Kernel32_GetLongPathNameA },
    { 01, 0, "GetLongPathNameW", Kernel32_GetLongPathNameW },
    { 01, 0, "GetModuleFileNameA", Kernel32_GetModuleFileNameA },
    { 01, 0, "GetModuleFileNameW", Kernel32_GetModuleFileNameW },
    { 01, 0, "GetModuleHandleA", Kernel32_GetModuleHandleA },
    { 01, 0, "GetModuleHandleW", Kernel32_GetModuleHandleW },
    { 01, 0, "GetModuleHandleExW", Kernel32_GetModuleHandleExW },
    { 01, 0, "GetOEMCP",Kernel32_GetOEMCP},
    { 01, 0, "GetNumberOfConsoleInputEvents", Kernel32_GetNumberOfConsoleInputEvents },
    { 01, 0, "GetProcAddress", Kernel32_GetProcAddress },
    { 01, 0, "GetProcessHeap", Kernel32_GetProcessHeap },
    { 01, 0, "GetStartupInfoA", Kernel32_GetStartupInfoA },
    { 01, 0, "GetStartupInfoW", Kernel32_GetStartupInfoW },
    { 01, 0, "GetStdHandle", Kernel32_GetStdHandle },
    { 01, 0, "GetStringTypeA", Kernel32_GetStringTypeA },
    { 01, 0, "GetStringTypeW", Kernel32_GetStringTypeW },
    { 01, 0, "GetSystemTimeAsFileTime", Kernel32_GetSystemTimeAsFileTime },
    { 01, 0, "GetTempPathA", Kernel32_GetTempPathA },
    { 01, 0, "GetThreadTimes", Kernel32_GetThreadTimes },
    { 01, 0, "GetTickCount", Kernel32_GetTickCount },
    { 01, 0, "GetTickCount64", Kernel32_GetTickCount64 },
	{ 01, 0, "GetTimeZoneInformation", Kernel32_GetTimeZoneInformation },
    { 01, 0, "GetVersion", Kernel32_GetVersion },
    { 01, 0, "GetVersionExA", Kernel32_GetVersionExA },
    { 01, 0, "GetVersionExW", Kernel32_GetVersionExW },
    { 01, 0, "GetVolumeInformationA", Kernel32_GetVolumeInformationA },
	{ 01, 0, "GlobalAlloc", Kernel32_GlobalAlloc },
	{ 01, 0, "GlobalFree", Kernel32_GlobalFree },
    { 01, 0, "GlobalMemoryStatus", Kernel32_GlobalMemoryStatus },
    { 01, 0, "HeapAlloc", Kernel32_HeapAlloc },
    { 01, 0, "HeapCreate", Kernel32_HeapCreate },
    { 01, 0, "HeapDestroy", Kernel32_HeapDestroy },
    { 01, 0, "HeapFree", Kernel32_HeapFree },
    { 01, 0, "HeapReAlloc", Kernel32_HeapReAlloc },
	{ 01, 0, "HeapValidate", Kernel32_HeapValidate }, 
	{ 01, 0, "HeapSetInformation", Kernel32_HeapSetInformation },
    { 01, 0, "HeapSize", Kernel32_HeapSize },
	{ 01, 0, "InitializeCriticalSection", Kernel32_InitializeCriticalSection },
    { 01, 0, "InitializeCriticalSectionAndSpinCount", Kernel32_InitializeCriticalSectionAndSpinCount },
    { 01, 0, "InitializeCriticalSectionEx", Kernel32_InitializeCriticalSectionEx },
    { 01, 0, "InitializeSListHead", Kernel32_InitializeSListHead },
    { 01, 0, "InitOnceExecuteOnce", Kernel32_InitOnceExecuteOnce },
    { 01, 0, "InterlockedCompareExchange", Kernel32_InterlockedCompareExchange },
    { 01, 0, "InterlockedDecrement", Kernel32_InterlockedDecrement },
    { 01, 0, "InterlockedExchange", Kernel32_InterlockedExchange },
    { 01, 0, "InterlockedIncrement", Kernel32_InterlockedIncrement },
	{ 01, 0, "IsBadReadPtr", Kernel32_IsBadReadPtr },
	{ 01, 0, "IsBadWritePtr", Kernel32_IsBadWritePtr },
    { 01, 0, "IsDBCSLeadByte", Kernel32_IsDBCSLeadByte },
    { 01, 0, "IsDebuggerPresent", Kernel32_IsDebuggerPresent },
    { 01, 0, "IsProcessorFeaturePresent", Kernel32_IsProcessorFeaturePresent },
    { 01, 0, "LCMapStringA", Kernel32_LCMapStringA },
    { 01, 0, "LCMapStringEx", Kernel32_LCMapStringEx },
    { 01, 0, "LCMapStringW", Kernel32_LCMapStringW },
    { 01, 0, "LeaveCriticalSection", Kernel32_LeaveCriticalSection },
    { 01, 0, "LoadLibraryA", Kernel32_LoadLibraryA },
	{ 01, 0, "LoadLibraryW", Kernel32_LoadLibraryW },
    { 01, 0, "LoadLibraryExW", Kernel32_LoadLibraryExW },
    { 01, 0, "lstrcpy", Kernel32_lstrcpy },
    { 01, 0, "lstrcmpiA", Kernel32_lstrcmpiA },
    { 01, 0, "lstrlenA", Kernel32_lstrlenA },
    { 01, 0, "MultiByteToWideChar", Kernel32_MultiByteToWideChar },
    { 01, 0, "PeekConsoleInputA", Kernel32_PeekConsoleInputA },
    { 01, 0, "QueryPerformanceCounter", Kernel32_QueryPerformanceCounter },
    { 01, 0, "RaiseException", Kernel32_RaiseException },
    { 01, 0, "ReadConsoleInputA", Kernel32_ReadConsoleInputA },
    { 01, 0, "ReadFile", Kernel32_ReadFile },
    { 01, 0, "RtlUnwind", Kernel32_RtlUnwind },
    { 01, 0, "SetFileAttributesA", Kernel32_SetFileAttributesA },
    { 01, 0, "SetConsoleCtrlHandler", Kernel32_SetConsoleCtrlHandler },
    { 01, 0, "SetConsoleMode", Kernel32_SetConsoleMode },
    { 01, 0, "SetConsoleTitleA", Kernel32_SetConsoleTitleA },
    { 01, 0, "SetErrorMode", Kernel32_SetErrorMode },
	{ 01, 0, "SetFileApisToOEM", Kernel32_SetFileApisToOEM },
    { 01, 0, "SetFilePointer", Kernel32_SetFilePointer },
    { 01, 0, "SetHandleCount", Kernel32_SetHandleCount },
    { 01, 0, "SetLastError", Kernel32_SetLastError },
    { 01, 0, "SetUnhandledExceptionFilter", Kernel32_SetUnhandledExceptionFilter },
    { 01, 0, "Sleep", Kernel32_Sleep },
    { 01, 0, "TerminateProcess", Kernel32_TerminateProcess },
    { 01, 0, "TlsAlloc", Kernel32_TlsAlloc },
    { 01, 0, "TlsFree", Kernel32_TlsFree },
    { 01, 0, "TlsGetValue", Kernel32_TlsGetValue },
    { 01, 0, "TlsSetValue", Kernel32_TlsSetValue },
    { 01, 0, "UnhandledExceptionFilter", Kernel32_UnhandledExceptionFilter },
    { 01, 0, "VirtualAlloc", Kernel32_VirtualAlloc },
    { 01, 0, "VirtualFree", Kernel32_VirtualFree },
	{ 01, 0, "WaitForSingleObject", Kernel32_WaitForSingleObject },
    { 01, 0, "WideCharToMultiByte", Kernel32_WideCharToMultiByte },
    { 01, 0, "WinExec", Kernel32_WinExec },
    { 01, 0, "WriteFile", Kernel32_WriteFile },
    
    
    /* user32.dll */
    { 02, 0, "AppendMenuA", User32_AppendMenuA },
    { 02, 0, "BeginPaint", User32_BeginPaint },
	{ 02, 0, "CharUpperW", User32_CharUpperW },
	{ 02, 0, "CreateDialogParamA", User32_CreateDialogParamA },
    { 02, 0, "CreateWindowExA", User32_CreateWindowExA },
    { 02, 0, "DefWindowProcA", User32_DefWindowProcA },
    { 02, 0, "DialogBoxParamA", User32_DialogBoxParamA },
    { 02, 0, "DispatchMessageA", User32_DispatchMessageA },
    { 02, 0, "DeleteMenu", User32_DeleteMenu },
    { 02, 0, "DestroyWindow", User32_DestroyWindow },
    { 02, 0, "DrawMenuBar", User32_DrawMenuBar },
    { 02, 0, "DrawTextA", User32_DrawTextA },
    { 02, 0, "EndDialog", User32_EndDialog },
    { 02, 0, "EndPaint", User32_EndPaint },
    { 02, 0, "EnumThreadWindows", User32_EnumThreadWindows },
	{ 02, 0, "FillRect", User32_FillRect },
    { 02, 0, "GetActiveWindow", User32_GetActiveWindow },
    { 02, 0, "GetDC", User32_GetDC },
    { 02, 0, "GetDlgItem", User32_GetDlgItem },
	{ 02, 0, "GetDlgItemInt", User32_GetDlgItemInt },
    { 02, 0, "GetDlgItemTextA",User32_GetDlgItemTextA},
    { 02, 0, "GetLastActivePopup", User32_GetLastActivePopup },
    { 02, 0, "GetMenu", User32_GetMenu },
    { 02, 0, "GetMessageA", User32_GetMessageA },
    { 02, 0, "GetProcessWindowStation", User32_GetProcessWindowStation },
    { 02, 0, "GetUserObjectInformationA", User32_GetUserObjectInformationA },
	{ 02, 0, "GetUserObjectInformationW", User32_GetUserObjectInformationW },
    { 02, 0, "GetWindowTextA", User32_GetWindowTextA },
	{ 02, 0, "GetWindowTextLengthA", User32_GetWindowTextLengthA },
    { 02, 0, "InvalidateRect", User32_InvalidateRect },
	{ 02, 0, "IsDlgButtonChecked", User32_IsDlgButtonChecked },
	{ 02, 0, "LoadIconA", User32_LoadIconA },
    { 02, 0, "LoadCursorA", User32_LoadCursorA },
    { 02, 0, "LoadMenuA", User32_LoadMenuA },
	{ 02, 0, "LoadStringA", User32_LoadStringA },
    { 02, 0, "MessageBoxA", User32_MessageBoxA },
	{ 02, 0, "MessageBoxW", User32_MessageBoxW },
    { 02, 0, "PeekMessageA", User32_PeekMessageA },
    { 02, 0, "PostQuitMessage", User32_PostQuitMessage },
    { 02, 0, "RegisterClassA", User32_RegisterClassA },
    { 02, 0, "RegisterClassExA", User32_RegisterClassExA },
	{ 02, 0, "ReleaseCapture", User32_ReleaseCapture },
    { 02, 0, "ReleaseDC", User32_ReleaseDC },
    { 02, 0, "ScrollWindow", User32_ScrollWindow },
    { 02, 0, "SendDlgItemMessageA", User32_SendDlgItemMessageA },
	{ 02, 0, "SendMessageA", User32_SendMessageA },
	{ 02, 0, "SetDlgItemTextA", User32_SetDlgItemTextA },
    { 02, 0, "SetFocus", User32_SetFocus },
    { 02, 0, "SetRect", User32_SetRect },
    { 02, 0, "SetScrollPos", User32_SetScrollPos },
    { 02, 0, "SetScrollRange", User32_SetScrollRange },
	{ 02, 0, "SetWindowLongA", User32_SetWindowLongA },
	{ 02, 0, "SetWindowPos", User32_SetWindowPos },
    { 02, 0, "SetWindowTextA", User32_SetWindowTextA },
    { 02, 0, "ShowWindow", User32_ShowWindow },
    { 02, 0, "TranslateMessage", User32_TranslateMessage },
    { 02, 0, "UpdateWindow", User32_UpdateWindow },
    { 02, 0, "ValidateRect", User32_ValidateRect },
    { 02, 0, "WinHelpA", User32_WinHelpA },
    { 02, 0, "wsprintfA", User32_wsprintfA },
    
    /* gdi32.dll */
    { 03, 0, "CreateFontA", Gdi32_CreateFontA },
	{ 03, 0, "CreatePen", Gdi32_CreatePen },
	{ 03, 0, "CreateSolidBrush", Gdi32_CreateSolidBrush },
    { 03, 0, "GetStockObject", Gdi32_GetStockObject },
    { 03, 0, "GetTextMetricsA", Gdi32_GetTextMetricsA },
	{ 03, 0, "RoundRect", Gdi32_RoundRect },
    { 03, 0, "SelectObject", Gdi32_SelectObject },
	{ 03, 0, "SetBkColor", Gdi32_SetBkColor },
	{ 03, 0, "SetBkMode", Gdi32_SetBkMode },
    { 03, 0, "SetTextColor", Gdi32_SetTextColor },

    /* PGORT90.dll */
    { 04, 0, "FindActCtxSectionStringW", Pgort90_FindActCtxSectionStringW }, 

    /* winmm.dll */
    { 05, 0, "waveOutClose", Winmm_waveOutClose },
    { 05, 0, "waveOutGetPosition", Winmm_waveOutGetPosition },
    { 05, 0, "waveOutOpen", Winmm_waveOutOpen },
    { 05, 0, "waveOutPause", Winmm_waveOutPause },
    { 05, 0, "waveOutPrepareHeader", Winmm_waveOutPrepareHeader },
    { 05, 0, "waveOutReset", Winmm_waveOutReset },
    { 05, 0, "waveOutRestart", Winmm_waveOutRestart },
    { 05, 0, "waveOutUnprepareHeader", Winmm_waveOutUnprepareHeader },
    { 05, 0, "waveOutWrite", Winmm_waveOutWrite },

    /* kernelbase.dll */
    { 06, 0, "GetLocaleInfoA", Kernelbase_GetLocaleInfoA },

    /* ntdll.dll */
    { 07, 0, "RtlGetNtVersionNumbers", Ntdll_RtlGetNtVersionNumbers },
    { 07, 0, "RtlUnwind", Ntdll_RtlUnwind },

    /* comctl32.dll */
    { 15, 0, "InitCommonControls", Comctl32_InitCommonControls },

    /* advapi32.dll */
    {  8, 0, "CryptAcquireContextW", Advapi32_CryptAcquireContextW },
    {  8, 0, "CryptGenRandom", Advapi32_CryptGenRandom },
    {  8, 0, "CryptReleaseContext",Advapi32_CryptReleaseContext },
    {  8, 0, "RegOpenKeyExA", Advapi32_RegOpenKeyExA },
    {  8, 0, "RegOpenKeyExW", Advapi32_RegOpenKeyExW },
    
    /* ws2_32.dll */
    { 16, 0x01, "accept", Ws2_32_accept },
    { 16, 0x02, "bind", Ws2_32_bind },
    { 16, 0x03, "closesocket", Ws2_32_closesocket },
    { 16, 0x04, "connect", Ws2_32_connect },
    { 16, 0x08, "htonl", Ws2_32_htonl },
    { 16, 0x09, "htons", Ws2_32_htons },
    { 16, 0x0b, "inet_addr", Ws2_32_inet_addr },
    { 16, 0x0d, "listen", Ws2_32_listen },
    { 16, 0x10, "recv", Ws2_32_recv },
    { 16, 0x13, "send", Ws2_32_send },
    { 16, 0x17, "socket", Ws2_32_socket },
    { 16, 0x73, "WSAStartup", Ws2_32_WSAStartup },
    { 16, 0x74, "WSACleanup", Ws2_32_WSACleanup },

    /* shell32.dll */
    { 18, 0, "SHGetMalloc", Shell32_SHGetMalloc },
    { 18, 0, "SHGetPathFromIDListA", Shell32_SHGetPathFromIDListA },
    { 18, 0, "SHGetSpecialFolderLocation", Shell32_SHGetSpecialFolderLocation },

	/* oleaut32.dll */
	{ 19, 0, "#9_VariantClear", Oleaut32_9_VariantClear },
	{ 19, 0x9, "VariantClear", Oleaut32_VariantClear },
    { 19, 0xA, "VariantCopy", Oleaut32_VariantCopy },
	{ 19, 0x2, "SysAllocString", Oleaut32_SysAllocString },
	{ 19, 0x96, "SysAllocStringByteLen", Oleaut32_SysAllocStringByteLen },
    { 19, 0x6, "SysFreeString", Oleaut32_SysFreeString },
};

uint WinAPINotAvailable(Processor *cpu)
{
    u32 target = cpu->Offset32(cpu->CurrentInst()->Main.Argument1);
    const ApiInfo *info = cpu->Proc()->GetApiInfoFromAddress(target);
    const char *dllName = "unknown";
    const char *funcName = "unknown";
    if (info) {
        dllName = info->ModuleName.c_str();
        funcName = info->FunctionName.c_str();
    }
    LxFatal("Windows API not available: [%08x] %s <%s:%s>\n", 
        cpu->EIP, cpu->CurrentInst()->Main.CompleteInstr,
        dllName, funcName);
    return 0;
}


LX_API uint QueryLibraryIndex( const char *dllName )
{
    static uint N = sizeof(WrappedLibraryTable) / sizeof(const char *);
	if (StrStrIA(dllName, "api-ms-win"))
		dllName = "kernel32.dll";		// quick hack for api-ms-... DLLs
    uint len = strlen(dllName);
    for (uint i = 0; i < N; i++) {
        if (!strnicmp(WrappedLibraryTable[i], dllName, len)) // Ignore case
            return i;
    }
    return 0; // Not found
}


LX_API uint QueryWinAPIIndexByName( const char *dllName, const char *funcName )
{
    HMODULE hModule = NULL;
    if (StrStrIA(dllName, "api-ms-win-core"))
        hModule = LxGetModuleHandle("kernel32.dll");
    else
        hModule = LxGetModuleHandle(dllName);
    uint idx = QueryWinAPIIndexByName(hModule, funcName);
    if (0 == idx) {
        LxWarning("Winapi %s::%s not found\n", dllName, funcName);
    }
    return idx;
}

LX_API uint QueryWinAPIIndexByName( HMODULE hModule, const char *funcName )
{
    static uint N = sizeof(WinAPIInfoTable) / sizeof(WinAPIInfo);

    uint dllIndex = LX_MODULE_NUM(hModule);
    if (dllIndex == 0) return 0; // dll not found
    for (uint i = 0; i < N; i++) {
        if (WinAPIInfoTable[i].DllIndex == dllIndex &&
            !strcmp(WinAPIInfoTable[i].FuncName, funcName)) // Don't ignore case
        {
            return i;
        }
    }
    return 0; // Not found 
}

LX_API uint QueryWinAPIIndexByOrdinal( const char *dllName, uint ordinal )
{
    HMODULE hModule = NULL;
    if (StrStrIA(dllName, "api-ms-win-core"))
        hModule = LxGetModuleHandle("kernel32.dll");
    else 
        hModule = LxGetModuleHandle(dllName);
    return QueryWinAPIIndexByOrdinal(hModule, ordinal);
}

LX_API uint QueryWinAPIIndexByOrdinal( HMODULE hModule, uint ordinal )
{
    static uint N = sizeof(WinAPIInfoTable) / sizeof(WinAPIInfo);
    uint dllIndex = LX_MODULE_NUM(hModule);
    if (dllIndex == 0) return 0;
    for (uint i = 0; i < N; i++) {
        if (WinAPIInfoTable[i].DllIndex == dllIndex && 
            WinAPIInfoTable[i].Ordinal == ordinal)
        {
            return i;
        }
    }
    return 0;
}

LX_API bool IsEmulatedLibrary( const char *dllName )
{
    if (StrStrIA(dllName, "api-ms-win-core")) return true; // quick hack for win7 system
    return QueryLibraryIndex(dllName) != 0;
}

LX_API HMODULE LxGetModuleHandle( const char *dllName )
{
    uint idx = QueryLibraryIndex(dllName);
    if (idx == 0) {
        LxWarning("No emulated module: %s\n", dllName);
        return (HMODULE) 0;
    }
    return (HMODULE) LX_MAKE_MODULE(idx);
}

LX_API const char *LxGetModuleName( HMODULE hModule )
{
    return WrappedLibraryTable[LX_MODULE_NUM(hModule)];
}

void CallWindowsAPI( Processor *cpu, u32 val )
{
    Assert(LX_IS_WINAPI(val));

    uint apiIndex = LX_WINAPI_NUM(val);

    if (cpu->Thr()) {
        cpu->Thr()->Plugins()->OnWinapiPreCall(cpu, apiIndex);
    }

    WinAPIHandler apiFunc = WinAPIInfoTable[apiIndex].Handler;

    uint r = apiFunc(cpu);

    if (cpu->Thr()) {
        cpu->Thr()->Plugins()->OnWinapiPostCall(cpu, apiIndex);
    }

    cpu->EIP = cpu->Pop32();
    cpu->ESP += r * 4;
}

LX_API const char * LxGetWinAPIModuleName( u32 index )
{
    const WinAPIInfo &info = WinAPIInfoTable[LX_WINAPI_NUM(index)];
    return WrappedLibraryTable[info.DllIndex];
}

LX_API const char * LxGetWinAPIName( u32 index )
{
    const WinAPIInfo &info = WinAPIInfoTable[LX_WINAPI_NUM(index)];
    return info.FuncName;
}

LX_API uint LxGetTotalWinAPIs()
{
    return sizeof(WinAPIInfoTable) / sizeof(WinAPIInfo);
}



END_NAMESPACE_LOCHSEMU()