#include "stdafx.h"
#include "Arietis.h"
#include "utilities.h"


static u64 file_time_2_utc(const FILETIME* ftime)
{
    LARGE_INTEGER li;
    Assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

int GetCpuUsage()
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

    int cpu = 0;

    GetSystemTimeAsFileTime(&now);

    if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time)) 
    {
        return 0;
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    system_time = (file_time_2_utc(&kernel_time) + 
        file_time_2_utc(&user_time)) / (int) sysInfo.dwNumberOfProcessors;
    time = file_time_2_utc(&now);

    if ((last_system_time_ == 0) || (last_time_ == 0))
    {
        last_system_time_ = system_time;
        last_time_ = time;
        return 0;
    }

    system_time_delta = system_time - last_system_time_;
    time_delta = time - last_time_;

    Assert(time_delta != 0);

    if (time_delta == 0)
        return 0;

    cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
    last_system_time_ = system_time;
    last_time_ = time;
    return max(cpu, 0);
}

int GetMemUsage() 
{
    PROCESS_MEMORY_COUNTERS_EX pmcx;
    if ( GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS) &pmcx, sizeof(pmcx)) ) {
        return (int) (pmcx.PrivateUsage / 1048576);
    }
    return 0;
}

int GetThreadCount() 
{
    DWORD pid = GetProcessId(GetCurrentProcess());
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

static uint BKDR_hash(const char *str)  
{  
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..  
    unsigned int hash = 0;  

    while (*str)  
    {  
        hash = hash * seed + (*str++);  
    }  

    return (hash & 0x7FFFFFFF);  
}  

uint StringHash( const char *str )
{
    return BKDR_hash(str);
}



bool Serializer::Serialize( ISerializable *obj, std::string &output )
{
    if (NULL == obj) return false;

    Json::Value root;
    obj->Serialize(root);

    Json::StyledWriter  writer;
    output = writer.write(root);
    return true;
}

bool Serializer::Deserialzie( ISerializable *obj, std::string &input )
{
    if (NULL == obj) return false;

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(input, root))
        return false;

    obj->Deserialize(root);
    return true;
}
