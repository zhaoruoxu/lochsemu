#include "stdafx.h"
#include "prophet.h"
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

uint FileTimeHash( const char *path )
{
    HANDLE hFile;
    FILETIME creation, lastWrite;

    hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == hFile) {
        LxFatal("Cannot open file: %s\n", path);
    }

    GetFileTime(hFile, &creation, NULL, &lastWrite);
    CloseHandle(hFile);

    return creation.dwHighDateTime ^ creation.dwLowDateTime ^ lastWrite.dwHighDateTime ^
        lastWrite.dwLowDateTime;
}

int CompareByteArray( cpbyte l, cpbyte r, int n )
{
    for (int i = 0; i < n; i++) {
        if (l[i] > r[i]) return 1;
        if (l[i] < r[i]) return -1;
    }
    return 0;
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

File::File( const char *filename, const char *mode )
{
    Open(filename, mode);
}

File::File(const std::string &filename, const std::string &mode)
{
    Open(filename.c_str(), mode.c_str());
}

void File::Open( const char *filename, const char *mode )
{
    m_fp = NULL;
    if (!filename || !mode) return;
    m_fp = fopen(filename, mode);
    if (m_fp == NULL) {
        LxFatal("Error creating file: %s mode: %s\n", filename, mode);
    }
}

File::~File()
{
    Close();
}

void File::Close()
{
    if (m_fp) {
        fclose(m_fp);
        m_fp = NULL;
    }
}


#define GRAPHVIZ_MULTITHREADED   0

DWORD __stdcall DotToImageRoutine(LPVOID lpParams)
{
    char dotbuf[1024], outbuf[MAX_PATH];
    const char *p = (const char *) lpParams;
    strcpy(outbuf, p);
    size_t l = strlen(outbuf);
    if (strcmp(outbuf + l - 4, ".dot") == 0) {
        outbuf[l-3] = 'p'; outbuf[l-2] = 'n'; outbuf[l-1] = 'g';
    }
    sprintf(dotbuf, "%%GRAPHVIZ%%\\dot.exe -Tpng %s -o %s", p, outbuf);
    system(dotbuf);
    delete [] p;      // p is 'new'ed
    return 0;
}

void DotToImage(const std::string &filename)
{
    char *p = new char[filename.size() + 1];  // use c++ allocation so that memory leaks can be detected
    strcpy(p, filename.c_str());

#if GRAPHVIZ_MULTITHREADED
    HANDLE hThread = CreateThread(NULL, 0, DotToImageRoutine, (LPVOID) p, 0, NULL);
    if (INVALID_HANDLE_VALUE == hThread) {
        LxError("Cannot create DotToImage thread\n");
    }
#else   // DOT_IN_ANOTHER_THREAD
    DotToImageRoutine((LPVOID) p);
#endif  // DOT_IN_ANOTHER_THREAD
}

std::string ByteArrayToDotString( cpbyte data, int n, int maxlen )
{
    std::stringstream ss;
    ss << "'";
    if (maxlen <= 0) maxlen = 0x7fffffff;
    for (int i = 0; i < min(n, maxlen); i++) {
        byte ch = data[i];
        if (ch == '\\') {
            ss << "\\\\";
        } else if (isprint(ch)) {
            ss << ch;
        } else {
            char buf[8];
            sprintf(buf, "\\\\%02x", ch);
            ss << buf;
        }
    }
    ss << "'";
    if (maxlen < n) {
         ss << "...";
    }
    return ss.str();
}
