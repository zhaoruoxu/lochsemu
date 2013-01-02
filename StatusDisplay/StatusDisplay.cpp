// StatusDisplay.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "StatusDisplay.h"
#include "config.h"
#include "emulator.h"
#include "process.h"

PluginHandle    g_handle;
HMODULE         g_module;
HANDLE          g_hThread; 
HANDLE          g_hProcess;
Config          g_config;
int             g_interval;
int             g_cpuCount;

#pragma comment(lib, "psapi.lib")

static DWORD __stdcall StatusDisplayThread(LPVOID lpParams);

STATUSDISPLAY_API bool LochsEmu_Plugin_Initialize( const LochsEmuInterface *lochsemu, PluginInfo *info )
{
    g_handle = lochsemu->Handle;
    strcpy_s(info->Name, sizeof(info->Name), "StatusDisplay");

    std::string cfgFile = LxGetModuleDirectory(g_module) + "status_display.ini";
    g_config.Initialize(cfgFile.c_str());

    if (!g_config.GetInt("General", "Enabled", 1)) {
        LxInfo("StatusDisplay is disabled\n");
        return false;
    }

    g_hProcess = GetCurrentProcess();
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    g_cpuCount = (int) sysInfo.dwNumberOfProcessors;


    g_interval = max(g_config.GetInt("General", "UpdateInterval", 1000), 50);
    g_hThread = CreateThread(NULL, 0, StatusDisplayThread, NULL, 0, NULL);
    if (g_hThread == INVALID_HANDLE_VALUE) {
        LxWarning("Cannot create status_display_thread\n");
        return false;
    }

    return true;
}

static u64 file_time_2_utc(const FILETIME* ftime)
{
    LARGE_INTEGER li;
    Assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

int get_cpu_usage()
{
    static i64 last_time_ = 0;
    static i64 last_system_time_ = 0;

    FILETIME now;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    i64 system_time;
    i64 time;
    i64 system_time_delta;
    i64 time_delta;

    int cpu = -1;

    GetSystemTimeAsFileTime(&now);

    if (!GetProcessTimes(g_hProcess, &creation_time, &exit_time, &kernel_time, &user_time)) 
    {
        return -1;
    }
    system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / g_cpuCount;
    time = file_time_2_utc(&now);

    if ((last_system_time_ == 0) || (last_time_ == 0))
    {
        last_system_time_ = system_time;
        last_time_ = time;
        return -1;
    }

    system_time_delta = system_time - last_system_time_;
    time_delta = time - last_time_;

    Assert(time_delta != 0);

    if (time_delta == 0)
        return -1;

    cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
    last_system_time_ = system_time;
    last_time_ = time;
    return cpu;
}

int GetMemUsage() 
{
    PROCESS_MEMORY_COUNTERS_EX pmcx;
    if ( GetProcessMemoryInfo(g_hProcess, (PPROCESS_MEMORY_COUNTERS) &pmcx, sizeof(pmcx)) ) {
        return (int) (pmcx.PrivateUsage / 1048576);
    }
    return 0;
}

int GetThreadCount() 
{
    DWORD pid = GetProcessId(g_hProcess);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) return -1;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return -1;
    }
    do {
        if (pe32.th32ProcessID == pid) {
            return pe32.cntThreads;
        }
    } while (Process32Next(hSnapshot, &pe32));
    CloseHandle(hSnapshot);
    return -1;
}

DWORD __stdcall StatusDisplayThread(LPVOID lpParams)
{
//    char buf[128];
//     while (true) {
//         sprintf(buf, "LOCHSEMU<%s> CPU: %d%%    MEM: %d MB    Threads: %d", 
//             LxEmulator.IsLoaded() ? LxEmulator.Proc()->GetModuleInfo(0)->Name : "NOT_LOADED", 
//             get_cpu_usage(), GetMemUsage(), GetThreadCount());
//         SetConsoleTitle(buf);
//         Sleep(g_interval);
//     }
    RunGUI();
    return 0;
}