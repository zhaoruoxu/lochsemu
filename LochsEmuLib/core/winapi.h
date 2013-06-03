#pragma once

#ifndef __CORE_WINAPI_H__
#define __CORE_WINAPI_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

/*
 * Windows API Numbering in LochsEmu
 * 0xff000000
 * 0xff000001
 * ...
 * 0xffffffff
 */

#define LX_WINAPI_MASK      0x00ffffff
#define LX_IS_WINAPI(x)     (((x) & ~LX_WINAPI_MASK) == ~LX_WINAPI_MASK)
#define LX_WINAPI_NUM(x)    ((x) & LX_WINAPI_MASK)
#define LX_MAKE_WINAPI_INDEX(x)     ((x) | ~LX_WINAPI_MASK)

#define LX_MODULE_MASK      0x000fffff
#define LX_IS_EMU_MODULE(x)     ((((u32) x) & ~LX_MODULE_MASK) == ~LX_MODULE_MASK)
#define LX_MODULE_NUM(x)    (((u32) x) & LX_MODULE_MASK)
#define LX_MAKE_MODULE(x)   ((x) | ~LX_MODULE_MASK)

#define PARAM(x)        cpu->GetStackParam32(x)
#define PARAM_PTR(x)    cpu->GetStackParamPtr32(x)
#define RET_VALUE       cpu->EAX
#define RET_PARAMS(x)   return (x)

// Return Value: number of parameters passed by stack
typedef uint (*WinAPIHandler)(Processor *cpu);

struct WinAPIInfo {
    uint DllIndex;
    uint Ordinal;
    char FuncName[64];
    WinAPIHandler Handler;
};

// Wrapped API table
extern WinAPIInfo WinAPIInfoTable[];
extern const char *WrappedLibraryTable[];

LX_API bool         IsEmulatedLibrary(const char *dllName);
LX_API uint         QueryWinAPIIndexByName(const char *dllName, const char *funcName);
LX_API uint         QueryWinAPIIndexByName(HMODULE hModule, const char *funcName);
LX_API uint         QueryWinAPIIndexByOrdinal(const char *dllName, uint ordinal);
LX_API uint         QueryWinAPIIndexByOrdinal(HMODULE hModule, uint ordinal);
LX_API uint         QueryLibraryIndex(const char *dllName);
LX_API uint         LxGetTotalWinAPIs();

LX_API HMODULE      LxGetModuleHandle(const char *dllName);
LX_API const char * LxGetModuleName(HMODULE hModule);

LX_API const char * LxGetWinAPIModuleName(u32 index);
LX_API const char * LxGetWinAPIName(u32 index);

void CallWindowsAPI(Processor *cpu, u32 val);


#define DECLARE_WINAPI_ENTRY(name)      uint name(Processor *cpu)

/************************************************************************/
/* N/A                                                                  */
/************************************************************************/
DECLARE_WINAPI_ENTRY(WinAPINotAvailable);

/************************************************************************/
/* Kernel32.dll                                                         */
/* Alphabetically, I hope                                               */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Kernel32_AddAtomA);
DECLARE_WINAPI_ENTRY(Kernel32_AreFileApisANSI);
DECLARE_WINAPI_ENTRY(Kernel32_CloseHandle);
//DECLARE_WINAPI_ENTRY(Kernel32_CloseToolhelp32Snapshot);
DECLARE_WINAPI_ENTRY(Kernel32_CompareStringW);
DECLARE_WINAPI_ENTRY(Kernel32_CreateEventA);
DECLARE_WINAPI_ENTRY(Kernel32_CreateEventW);
DECLARE_WINAPI_ENTRY(Kernel32_CreateFileA);
DECLARE_WINAPI_ENTRY(Kernel32_CreateFileW);
DECLARE_WINAPI_ENTRY(kernel32_CreateProcessA);
DECLARE_WINAPI_ENTRY(Kernel32_CreateSemaphoreA);
DECLARE_WINAPI_ENTRY(Kernel32_CreateThread);
DECLARE_WINAPI_ENTRY(Kernel32_CreateToolhelp32Snapshot);
DECLARE_WINAPI_ENTRY(Kernel32_DecodePointer);
DECLARE_WINAPI_ENTRY(Kernel32_DeleteCriticalSection);
DECLARE_WINAPI_ENTRY(Kernel32_DeleteFileA);
DECLARE_WINAPI_ENTRY(Kernel32_DisableThreadLibraryCalls);
DECLARE_WINAPI_ENTRY(Kernel32_EncodePointer);
DECLARE_WINAPI_ENTRY(Kernel32_EnterCriticalSection);
DECLARE_WINAPI_ENTRY(Kernel32_ExitProcess);
DECLARE_WINAPI_ENTRY(Kernel32_ExitThread);
DECLARE_WINAPI_ENTRY(Kernel32_FileTimeToLocalFileTime);
DECLARE_WINAPI_ENTRY(Kernel32_FindActCtxSectionStringW);
DECLARE_WINAPI_ENTRY(Kernel32_FindAtomA);
DECLARE_WINAPI_ENTRY(Kernel32_FindClose);
DECLARE_WINAPI_ENTRY(Kernel32_FindFirstFileW);
DECLARE_WINAPI_ENTRY(Kernel32_FindResourceExA);
DECLARE_WINAPI_ENTRY(Kernel32_FlsAlloc);
DECLARE_WINAPI_ENTRY(Kernel32_FlsGetValue);
DECLARE_WINAPI_ENTRY(Kernel32_FlsSetValue);
DECLARE_WINAPI_ENTRY(Kernel32_FlsFree);
DECLARE_WINAPI_ENTRY(Kernel32_FlushConsoleInputBuffer);
DECLARE_WINAPI_ENTRY(Kernel32_FreeEnvironmentStringsW);
DECLARE_WINAPI_ENTRY(Kernel32_FreeLibrary);
DECLARE_WINAPI_ENTRY(Kernel32_GetACP);
DECLARE_WINAPI_ENTRY(Kernel32_GetAtomNameA);
DECLARE_WINAPI_ENTRY(Kernel32_GetCPInfo);
DECLARE_WINAPI_ENTRY(Kernel32_GetCommandLineA);
DECLARE_WINAPI_ENTRY(Kernel32_GetCommandLineW);
DECLARE_WINAPI_ENTRY(Kernel32_GetConsoleCP);
DECLARE_WINAPI_ENTRY(Kernel32_GetConsoleMode);
DECLARE_WINAPI_ENTRY(Kernel32_GetComputerNameA);
DECLARE_WINAPI_ENTRY(Kernel32_GetCurrentDirectoryW);
//DECLARE_WINAPI_ENTRY(Kernel32_GetCurrentPackageId);
DECLARE_WINAPI_ENTRY(Kernel32_GetCurrentProcess);
DECLARE_WINAPI_ENTRY(Kernel32_GetCurrentProcessId);
DECLARE_WINAPI_ENTRY(Kernel32_GetCurrentThreadId);
DECLARE_WINAPI_ENTRY(Kernel32_GetCurrentThread);
DECLARE_WINAPI_ENTRY(Kernel32_GetDriveTypeA);
DECLARE_WINAPI_ENTRY(Kernel32_GetEnvironmentStrings);
DECLARE_WINAPI_ENTRY(Kernel32_GetEnvironmentStringsW);
DECLARE_WINAPI_ENTRY(Kernel32_GetEnvironmentVariableA);
DECLARE_WINAPI_ENTRY(Kernel32_GetExitCodeProcess);
DECLARE_WINAPI_ENTRY(Kernel32_GetFileAttributesA);
DECLARE_WINAPI_ENTRY(Kernel32_GetFileType);
DECLARE_WINAPI_ENTRY(Kernel32_GetLastError);
DECLARE_WINAPI_ENTRY(Kernel32_GetLocaleInfoA);
DECLARE_WINAPI_ENTRY(Kernel32_GetLocaleInfoW);
DECLARE_WINAPI_ENTRY(Kernel32_GetLocalTime);
DECLARE_WINAPI_ENTRY(Kernel32_GetLongPathNameA);
DECLARE_WINAPI_ENTRY(Kernel32_GetLongPathNameW);
DECLARE_WINAPI_ENTRY(Kernel32_GetModuleFileNameA);
DECLARE_WINAPI_ENTRY(Kernel32_GetModuleFileNameW);
DECLARE_WINAPI_ENTRY(Kernel32_GetModuleHandleA);
DECLARE_WINAPI_ENTRY(Kernel32_GetModuleHandleW);
DECLARE_WINAPI_ENTRY(Kernel32_GetModuleHandleExW);
DECLARE_WINAPI_ENTRY(Kernel32_GetNumberOfConsoleInputEvents);
DECLARE_WINAPI_ENTRY(Kernel32_GetOEMCP);
DECLARE_WINAPI_ENTRY(Kernel32_GetProcAddress);
DECLARE_WINAPI_ENTRY(Kernel32_GetProcessHeap);
DECLARE_WINAPI_ENTRY(Kernel32_GetStartupInfoA);
DECLARE_WINAPI_ENTRY(Kernel32_GetStartupInfoW);
DECLARE_WINAPI_ENTRY(Kernel32_GetStdHandle);
DECLARE_WINAPI_ENTRY(Kernel32_GetStringTypeA);
DECLARE_WINAPI_ENTRY(Kernel32_GetStringTypeW);
DECLARE_WINAPI_ENTRY(Kernel32_GetSystemTimeAsFileTime);
DECLARE_WINAPI_ENTRY(Kernel32_GetTempPathA);
DECLARE_WINAPI_ENTRY(Kernel32_GetThreadTimes);
DECLARE_WINAPI_ENTRY(Kernel32_GetTickCount);
DECLARE_WINAPI_ENTRY(Kernel32_GetTickCount64);
DECLARE_WINAPI_ENTRY(Kernel32_GetTimeZoneInformation);
DECLARE_WINAPI_ENTRY(Kernel32_GetUserDefaultLCID);
DECLARE_WINAPI_ENTRY(Kernel32_GetVersion);
DECLARE_WINAPI_ENTRY(Kernel32_GetVersionExA);
DECLARE_WINAPI_ENTRY(Kernel32_GetVersionExW);
DECLARE_WINAPI_ENTRY(Kernel32_GetVolumeInformationA);
DECLARE_WINAPI_ENTRY(Kernel32_GlobalAlloc);
DECLARE_WINAPI_ENTRY(Kernel32_GlobalFree);
DECLARE_WINAPI_ENTRY(Kernel32_GlobalMemoryStatus);
DECLARE_WINAPI_ENTRY(Kernel32_Heap32First);
DECLARE_WINAPI_ENTRY(Kernel32_Heap32Next);
DECLARE_WINAPI_ENTRY(Kernel32_Heap32ListFirst);
DECLARE_WINAPI_ENTRY(Kernel32_Heap32ListNext);
DECLARE_WINAPI_ENTRY(Kernel32_HeapAlloc);
DECLARE_WINAPI_ENTRY(Kernel32_HeapCreate);
DECLARE_WINAPI_ENTRY(Kernel32_HeapDestroy);
DECLARE_WINAPI_ENTRY(Kernel32_HeapFree);
DECLARE_WINAPI_ENTRY(Kernel32_HeapReAlloc);
DECLARE_WINAPI_ENTRY(Kernel32_HeapValidate);
DECLARE_WINAPI_ENTRY(Kernel32_HeapSetInformation);
DECLARE_WINAPI_ENTRY(Kernel32_HeapSize);
DECLARE_WINAPI_ENTRY(Kernel32_InitializeCriticalSection);
DECLARE_WINAPI_ENTRY(Kernel32_InitializeCriticalSectionAndSpinCount);
DECLARE_WINAPI_ENTRY(Kernel32_InitializeCriticalSectionEx);
DECLARE_WINAPI_ENTRY(Kernel32_InitializeSListHead);
DECLARE_WINAPI_ENTRY(Kernel32_InitOnceExecuteOnce);
DECLARE_WINAPI_ENTRY(Kernel32_InterlockedCompareExchange);
DECLARE_WINAPI_ENTRY(Kernel32_InterlockedDecrement);
DECLARE_WINAPI_ENTRY(Kernel32_InterlockedExchange);
DECLARE_WINAPI_ENTRY(Kernel32_InterlockedIncrement);
DECLARE_WINAPI_ENTRY(Kernel32_IsBadReadPtr);
DECLARE_WINAPI_ENTRY(Kernel32_IsBadWritePtr);
DECLARE_WINAPI_ENTRY(Kernel32_IsDBCSLeadByte);
DECLARE_WINAPI_ENTRY(Kernel32_IsDebuggerPresent);
DECLARE_WINAPI_ENTRY(Kernel32_IsProcessorFeaturePresent);
DECLARE_WINAPI_ENTRY(Kernel32_IsValidCodePage);
DECLARE_WINAPI_ENTRY(Kernel32_IsValidLocale);
DECLARE_WINAPI_ENTRY(Kernel32_LCMapStringA);
DECLARE_WINAPI_ENTRY(Kernel32_LCMapStringEx);
DECLARE_WINAPI_ENTRY(Kernel32_LCMapStringW);
DECLARE_WINAPI_ENTRY(Kernel32_LeaveCriticalSection);
DECLARE_WINAPI_ENTRY(Kernel32_LoadLibraryA);
DECLARE_WINAPI_ENTRY(Kernel32_LoadLibraryW);
DECLARE_WINAPI_ENTRY(Kernel32_LoadLibraryExW);
DECLARE_WINAPI_ENTRY(Kernel32_lstrcpy);
DECLARE_WINAPI_ENTRY(Kernel32_lstrcmpiA);
DECLARE_WINAPI_ENTRY(Kernel32_lstrlenA);
DECLARE_WINAPI_ENTRY(Kernel32_Module32First);
DECLARE_WINAPI_ENTRY(Kernel32_Module32Next);
DECLARE_WINAPI_ENTRY(Kernel32_MultiByteToWideChar);
DECLARE_WINAPI_ENTRY(Kernel32_PeekConsoleInputA);
DECLARE_WINAPI_ENTRY(Kernel32_Process32First);
DECLARE_WINAPI_ENTRY(Kernel32_Process32Next);
DECLARE_WINAPI_ENTRY(Kernel32_QueryPerformanceCounter);
DECLARE_WINAPI_ENTRY(Kernel32_RaiseException);
DECLARE_WINAPI_ENTRY(Kernel32_ReadConsoleInputA);
DECLARE_WINAPI_ENTRY(Kernel32_ReadFile);
DECLARE_WINAPI_ENTRY(Kernel32_ResumeThread);
DECLARE_WINAPI_ENTRY(Kernel32_RtlUnwind);
DECLARE_WINAPI_ENTRY(Kernel32_SearchPathA);
DECLARE_WINAPI_ENTRY(Kernel32_SetFileAttributesA);
DECLARE_WINAPI_ENTRY(Kernel32_SetConsoleCtrlHandler);
DECLARE_WINAPI_ENTRY(Kernel32_SetConsoleMode);
DECLARE_WINAPI_ENTRY(Kernel32_SetConsoleTitleA);
DECLARE_WINAPI_ENTRY(Kernel32_SetErrorMode);
DECLARE_WINAPI_ENTRY(Kernel32_SetEvent);
DECLARE_WINAPI_ENTRY(Kernel32_SetFileApisToOEM);
DECLARE_WINAPI_ENTRY(Kernel32_SetFilePointer);
DECLARE_WINAPI_ENTRY(Kernel32_SetHandleCount);
DECLARE_WINAPI_ENTRY(Kernel32_SetLastError);
DECLARE_WINAPI_ENTRY(Kernel32_SetUnhandledExceptionFilter);
DECLARE_WINAPI_ENTRY(Kernel32_Sleep);
DECLARE_WINAPI_ENTRY(Kernel32_SleepEx);
DECLARE_WINAPI_ENTRY(Kernel32_TerminateProcess);
DECLARE_WINAPI_ENTRY(Kernel32_Thread32First);
DECLARE_WINAPI_ENTRY(Kernel32_Thread32Next);
DECLARE_WINAPI_ENTRY(Kernel32_TlsAlloc);
DECLARE_WINAPI_ENTRY(Kernel32_TlsGetValue);
DECLARE_WINAPI_ENTRY(Kernel32_TlsSetValue);
DECLARE_WINAPI_ENTRY(Kernel32_TlsFree);
DECLARE_WINAPI_ENTRY(Kernel32_UnhandledExceptionFilter);
DECLARE_WINAPI_ENTRY(Kernel32_VirtualAlloc);
DECLARE_WINAPI_ENTRY(Kernel32_VirtualFree);
DECLARE_WINAPI_ENTRY(Kernel32_WaitForSingleObject);
DECLARE_WINAPI_ENTRY(Kernel32_WideCharToMultiByte);
DECLARE_WINAPI_ENTRY(Kernel32_WinExec);
DECLARE_WINAPI_ENTRY(Kernel32_WriteFile);


/************************************************************************/
/* User32.dll                                                           */
/************************************************************************/
DECLARE_WINAPI_ENTRY(User32_AppendMenuA);
DECLARE_WINAPI_ENTRY(User32_BeginPaint);
DECLARE_WINAPI_ENTRY(User32_CharNextA);
DECLARE_WINAPI_ENTRY(User32_CharUpperW);
DECLARE_WINAPI_ENTRY(User32_CreateDialogParamA);
DECLARE_WINAPI_ENTRY(User32_CreateWindowExA);
DECLARE_WINAPI_ENTRY(User32_DialogBoxParamA);
DECLARE_WINAPI_ENTRY(User32_DispatchMessageA);
DECLARE_WINAPI_ENTRY(User32_DefWindowProcA);
DECLARE_WINAPI_ENTRY(User32_DeleteMenu);
DECLARE_WINAPI_ENTRY(User32_DestroyWindow);
DECLARE_WINAPI_ENTRY(User32_DrawMenuBar);
DECLARE_WINAPI_ENTRY(User32_DrawTextA);
DECLARE_WINAPI_ENTRY(User32_EndDialog);
DECLARE_WINAPI_ENTRY(User32_EndPaint);
DECLARE_WINAPI_ENTRY(User32_EnumThreadWindows);
DECLARE_WINAPI_ENTRY(User32_FillRect);
DECLARE_WINAPI_ENTRY(User32_GetActiveWindow);
DECLARE_WINAPI_ENTRY(User32_GetClassInfoA);
DECLARE_WINAPI_ENTRY(User32_GetCursorInfo);
DECLARE_WINAPI_ENTRY(User32_GetDC);
DECLARE_WINAPI_ENTRY(User32_GetDesktopWindow);
DECLARE_WINAPI_ENTRY(User32_GetDlgItem);
DECLARE_WINAPI_ENTRY(User32_GetDlgItemInt);
DECLARE_WINAPI_ENTRY(User32_GetDlgItemTextA);
DECLARE_WINAPI_ENTRY(User32_GetForegroundWindow);
DECLARE_WINAPI_ENTRY(User32_GetKeyboardType);
DECLARE_WINAPI_ENTRY(User32_GetLastActivePopup);
DECLARE_WINAPI_ENTRY(User32_GetMenu);
DECLARE_WINAPI_ENTRY(User32_GetMessageA);
DECLARE_WINAPI_ENTRY(User32_GetProcessWindowStation);
DECLARE_WINAPI_ENTRY(User32_GetQueueStatus);
DECLARE_WINAPI_ENTRY(User32_GetUserObjectInformationA);
DECLARE_WINAPI_ENTRY(User32_GetUserObjectInformationW);
DECLARE_WINAPI_ENTRY(User32_GetWindowTextA);
DECLARE_WINAPI_ENTRY(User32_GetWindowTextLengthA);
DECLARE_WINAPI_ENTRY(User32_InvalidateRect);
DECLARE_WINAPI_ENTRY(User32_IsDlgButtonChecked);
DECLARE_WINAPI_ENTRY(User32_LoadIconA);
DECLARE_WINAPI_ENTRY(User32_LoadCursorA);
DECLARE_WINAPI_ENTRY(User32_LoadMenuA);
DECLARE_WINAPI_ENTRY(User32_LoadStringA);
DECLARE_WINAPI_ENTRY(User32_MessageBoxA);
DECLARE_WINAPI_ENTRY(User32_MessageBoxW);
DECLARE_WINAPI_ENTRY(User32_PeekMessageA);
DECLARE_WINAPI_ENTRY(User32_PostQuitMessage);
DECLARE_WINAPI_ENTRY(User32_RegisterClassA);
DECLARE_WINAPI_ENTRY(User32_RegisterClassExA);
DECLARE_WINAPI_ENTRY(User32_ReleaseCapture);
DECLARE_WINAPI_ENTRY(User32_ReleaseDC);
DECLARE_WINAPI_ENTRY(User32_ScrollWindow);
DECLARE_WINAPI_ENTRY(User32_SendDlgItemMessageA);
DECLARE_WINAPI_ENTRY(User32_SendMessageA);
DECLARE_WINAPI_ENTRY(User32_SetDlgItemTextA);
DECLARE_WINAPI_ENTRY(User32_SetFocus);
DECLARE_WINAPI_ENTRY(User32_SetRect);
DECLARE_WINAPI_ENTRY(User32_SetScrollPos);
DECLARE_WINAPI_ENTRY(User32_SetScrollRange);
DECLARE_WINAPI_ENTRY(User32_SetWindowLongA);
DECLARE_WINAPI_ENTRY(User32_SetWindowPos);
DECLARE_WINAPI_ENTRY(User32_SetWindowTextA);
DECLARE_WINAPI_ENTRY(User32_ShowWindow);
DECLARE_WINAPI_ENTRY(User32_TranslateMessage);
DECLARE_WINAPI_ENTRY(User32_UpdateWindow);
DECLARE_WINAPI_ENTRY(User32_ValidateRect);
DECLARE_WINAPI_ENTRY(User32_WinHelpA);
DECLARE_WINAPI_ENTRY(User32_wsprintfA);

/************************************************************************/
/* gdi32.dll                                                            */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Gdi32_CreateFontA);
DECLARE_WINAPI_ENTRY(Gdi32_CreatePen);
DECLARE_WINAPI_ENTRY(Gdi32_CreateSolidBrush);
DECLARE_WINAPI_ENTRY(Gdi32_GetStockObject);
DECLARE_WINAPI_ENTRY(Gdi32_GetTextMetricsA);
DECLARE_WINAPI_ENTRY(Gdi32_RoundRect);
DECLARE_WINAPI_ENTRY(Gdi32_SelectObject);
DECLARE_WINAPI_ENTRY(Gdi32_SetBkColor);
DECLARE_WINAPI_ENTRY(Gdi32_SetBkMode);
DECLARE_WINAPI_ENTRY(Gdi32_SetTextColor);

/************************************************************************/
/* Pgort90.dll                                                          */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Pgort90_FindActCtxSectionStringW);

/************************************************************************/
/* mscoree.dll                                                          */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Mscoree_CorExitProcess);

/************************************************************************/
/* winmm.dll                                                            */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Winmm_waveOutClose);
DECLARE_WINAPI_ENTRY(Winmm_waveOutGetPosition);
DECLARE_WINAPI_ENTRY(Winmm_waveOutOpen);
DECLARE_WINAPI_ENTRY(Winmm_waveOutPause);
DECLARE_WINAPI_ENTRY(Winmm_waveOutPrepareHeader);
DECLARE_WINAPI_ENTRY(Winmm_waveOutReset);
DECLARE_WINAPI_ENTRY(Winmm_waveOutRestart);
DECLARE_WINAPI_ENTRY(Winmm_waveOutUnprepareHeader);
DECLARE_WINAPI_ENTRY(Winmm_waveOutWrite);

/************************************************************************/
/* Kernelbase.dll                                                       */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Kernelbase_GetLocaleInfoA);

/************************************************************************/
/* ntdll.dll                                                            */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Ntdll_RtlGetNtVersionNumbers);
DECLARE_WINAPI_ENTRY(Ntdll_RtlUnwind);

/************************************************************************/
/* comctl32.dll                                                         */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Comctl32_InitCommonControls);

/************************************************************************/
/* advapi32.dll                                                         */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Advapi32_CryptAcquireContextW);
DECLARE_WINAPI_ENTRY(Advapi32_CryptGenRandom);
DECLARE_WINAPI_ENTRY(Advapi32_CryptReleaseContext);
DECLARE_WINAPI_ENTRY(Advapi32_RegCloseKey);
DECLARE_WINAPI_ENTRY(Advapi32_RegEnumKeyExA);
DECLARE_WINAPI_ENTRY(Advapi32_RegOpenKeyExA);
DECLARE_WINAPI_ENTRY(Advapi32_RegOpenKeyExW);
DECLARE_WINAPI_ENTRY(Advapi32_RegQueryInfoKeyA);
DECLARE_WINAPI_ENTRY(Advapi32_RegQueryValueExA);

/************************************************************************/
/* shell32.dll                                                          */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Shell32_SHGetMalloc);
DECLARE_WINAPI_ENTRY(Shell32_SHGetSpecialFolderLocation);
DECLARE_WINAPI_ENTRY(Shell32_SHGetPathFromIDListA);

/************************************************************************/
/* oleaut32.dll                                                         */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Oleaut32_9_VariantClear);
DECLARE_WINAPI_ENTRY(Oleaut32_VariantClear);
DECLARE_WINAPI_ENTRY(Oleaut32_VariantCopy);
DECLARE_WINAPI_ENTRY(Oleaut32_SysAllocString);
DECLARE_WINAPI_ENTRY(Oleaut32_SysAllocStringByteLen);
DECLARE_WINAPI_ENTRY(Oleaut32_SysFreeString);

/************************************************************************/
/* ws2_32.dll                                                           */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Ws2_32___WSAFDIsSet);
DECLARE_WINAPI_ENTRY(Ws2_32_accept);
DECLARE_WINAPI_ENTRY(Ws2_32_bind);
DECLARE_WINAPI_ENTRY(Ws2_32_closesocket);
DECLARE_WINAPI_ENTRY(Ws2_32_connect);
DECLARE_WINAPI_ENTRY(Ws2_32_freeaddrinfo);
DECLARE_WINAPI_ENTRY(Ws2_32_getaddrinfo);
DECLARE_WINAPI_ENTRY(Ws2_32_gethostbyaddr);
DECLARE_WINAPI_ENTRY(Ws2_32_gethostbyname);
DECLARE_WINAPI_ENTRY(Ws2_32_gethostname);
DECLARE_WINAPI_ENTRY(Ws2_32_getpeername);
DECLARE_WINAPI_ENTRY(Ws2_32_getservbyname);
DECLARE_WINAPI_ENTRY(Ws2_32_getsockname);
DECLARE_WINAPI_ENTRY(Ws2_32_getsockopt);
DECLARE_WINAPI_ENTRY(Ws2_32_htonl);
DECLARE_WINAPI_ENTRY(Ws2_32_htons);
DECLARE_WINAPI_ENTRY(Ws2_32_inet_addr);
DECLARE_WINAPI_ENTRY(Ws2_32_ioctlsocket);
DECLARE_WINAPI_ENTRY(Ws2_32_listen);
DECLARE_WINAPI_ENTRY(Ws2_32_ntohs);
DECLARE_WINAPI_ENTRY(Ws2_32_recv);
DECLARE_WINAPI_ENTRY(Ws2_32_recvfrom);
DECLARE_WINAPI_ENTRY(Ws2_32_send);
DECLARE_WINAPI_ENTRY(Ws2_32_sendto);
DECLARE_WINAPI_ENTRY(Ws2_32_select);
DECLARE_WINAPI_ENTRY(Ws2_32_setsockopt);
DECLARE_WINAPI_ENTRY(Ws2_32_shutdown);
DECLARE_WINAPI_ENTRY(Ws2_32_socket);
DECLARE_WINAPI_ENTRY(Ws2_32_WSACleanup);
DECLARE_WINAPI_ENTRY(Ws2_32_WSAGetLastError);
DECLARE_WINAPI_ENTRY(Ws2_32_WSASetLastError);
DECLARE_WINAPI_ENTRY(Ws2_32_WSAStartup);

/************************************************************************/
/* netapi32.dll                                                         */
/************************************************************************/
DECLARE_WINAPI_ENTRY(Netapi32_NetStatisticsGet);
DECLARE_WINAPI_ENTRY(Netapi32_NetApiBufferFree);

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_WINAPI_H__
