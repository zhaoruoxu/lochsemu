#include "stdafx.h"
#include "lochsemu.h"
#include "logger.h"
#include "emulator.h"
#include "config.h"

#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../3rdparty/BeaEngine/BeaEngine.h"

#include "../3rdparty/libdasm/libdasm.h"
#include "instruction.h"

BEGIN_NAMESPACE_LOCHSEMU()

Config      LxConfig;
Emulator    LxEmulator;

static WORD LxChangeConsoleMode( WORD mode );

static void InitConfig();

bool LxInit()
{
    InitConfig();
    std::string logFile = LxGetRuntimeDirectory() + "lochsemu.log";
    Log::Instance()->OpenLogFile(logFile.c_str(), true);

    if (LxConfig.GetInt("General", "DetectMemoryLeaks", 1) != 0) {
        int dbgFlags;
        dbgFlags = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
        dbgFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
        dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
        _CrtSetDbgFlag( dbgFlags);
    }

    return true;
}

void InitConfig()
{
    LxConfig.Initialize("lochsemu.ini");
}

WORD LxChangeConsoleMode( WORD mode )
{
    WORD wOldMode;
    HANDLE hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

    hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
    if ( INVALID_HANDLE_VALUE == hStdOut ) {
        exit( 1 );
    }
    if ( !GetConsoleScreenBufferInfo( hStdOut, &csbiInfo ) ) {
        exit( 1 );
    }
    wOldMode = csbiInfo.wAttributes;
    if ( !SetConsoleTextAttribute( hStdOut, mode ) ) {
        exit( 1 );
    }
    return wOldMode;
}

LX_API void LxFormatTimeMS(DWORD time, char *buf)
{
    int ms = time % 1000;
    time /= 1000;
    int s = time % 60;
    time /= 60;
    int m = time % 60;
    time /= 60;
    int h = time;
    if (h) {
        sprintf(buf, "%02dh %02dm %02ds %03dms", h, m, s, ms);
    } else if (m) {
        sprintf(buf, "%02dm %02ds %03dms", m, s, ms);
    } else if (s) {
        sprintf(buf, "%02ds %03dms", s, ms);
    } else {
        sprintf(buf, "%03dms", ms);
    }
}

LX_API void LxFormatTimeS(DWORD time, char *buf)
{
    time /= 1000;
    int s = time % 60;
    time /= 60;
    int m = time % 60;
    time /= 60;
    int h = time;
    if (h) {
        sprintf(buf, "%02dh %02dm %02ds", h, m, s);
    } else if (m) {
        sprintf(buf, "%02dm %02ds", m, s);
    } else {
        sprintf(buf, "%02ds", s);
    } 
}

LX_API void LxGetDateTime(char *buffer) {
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, 
        sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
}

void LxLogInternal(LogLevel level, const char *format, va_list args)
{
    static WORD consoleMode[] = {
        FOREGROUND_GREEN,
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        //FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE,
        BACKGROUND_RED | BACKGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
    };
    static const char *logFormat[] = {
        "[*][%s] %s",
        "[-][%s] %s",
        "[?][%s] %s",
        "[!][%s] %s",
    };
    static const char *prompt[] = {
        "[*] ", "[-] ", "[?] ", "[!] ",
    };

    SyncObjectLock lock(LxEmulator);

    WORD wOldMode;
    char dtBuffer[1024];
    char buffer[16384];

    LxGetDateTime(dtBuffer);
    vsprintf(buffer, format, args);

    wOldMode = LxChangeConsoleMode(consoleMode[level]);
    printf(prompt[level]);
    //LxChangeConsoleMode( FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf(buffer);
    fflush(stdout);
    LxChangeConsoleMode( wOldMode );

    /*
     * Log to file even not in debug mode
     */
    Log::Instance()->Log(logFormat[level], dtBuffer, buffer);
}

LX_API void LxInfo(const char *format, ...) {
    va_list args;
    va_start(args, format);
    LxLogInternal(LOG_LEVEL_INFO, format, args);
    va_end(args);
}

LX_API void _LxDebug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    LxLogInternal(LOG_LEVEL_DEBUG, format, args);
    va_end(args);
}

LX_API void LxWarning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    LxLogInternal(LOG_LEVEL_WARNING, format, args);
    va_end(args);
}

LX_API void LxError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    LxLogInternal(LOG_LEVEL_FATAL, format, args);
    va_end(args);
}

LX_API void LxFatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    LxLogInternal(LOG_LEVEL_FATAL, format, args);
    va_end(args);

    Assert(0);
    DebugBreak();
    exit(EXIT_FAILURE);
}

LX_API bool LxFileExists( LPCSTR filepath )
{
    return PathFileExistsA(filepath) != 0;
}

LX_API bool LxWideToByte( LPCWSTR w, LPSTR b, int len)
{
    int lenW = wcslen(w);
    B_RETURN( lenW <= len );
    int lenA = WideCharToMultiByte(CP_ACP, 0, w, lenW, 0, 0, NULL, NULL);
    B_RETURN( lenA <= len );
    if (lenA > 0) {
        WideCharToMultiByte(CP_ACP, 0, w, lenW, b, lenA, NULL, NULL);
        b[lenA] = 0;
    } else {
        LxFatal("Unable to convert to ANSI string\n");
        return false;
    }
    return true;
}

LX_API bool LxByteToWide( LPCSTR b, LPWSTR w, int len)
{
    int lenA = strlen(b);
    B_RETURN( lenA <= len );
    int lenW = MultiByteToWideChar(CP_ACP, 0, b, lenA, 0, 0);
    B_RETURN( lenW <= len );
    if (lenW > 0) {
        MultiByteToWideChar(CP_ACP, 0, b, lenA, w, lenW);
        w[lenW] = 0;
    } else {
        LxFatal("Unable to convert to wide string\n");
        return false;
    }
    return true;
}

INLINE LX_API bool LxDecode( byte data[], Instruction *inst, u32 eip )
{
    __try {
        get_instruction(&inst->Aux, data, MODE_32);
        inst->Main.EIP = (UIntPtr) data;
        inst->Main.VirtualAddr = eip;
        inst->Length = Disasm((LPDISASM) &inst->Main);
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
    return true;
}


LX_API INLINE u32 LxRegConvert( u32 beaReg )
{
    static u32 RegTable[0x81] = {
        /* 0x00 - 0x0f */ 0xff, 0x00, 0x01, 0xff, 0x02, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        /* 0x10 - 0x1f */ 0x04, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        /* 0x20 - 0x2f */ 0x05, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        /* 0x30 - 0x3f */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        /* 0x40 - 0x4f */ 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        /* 0x50 - 0x5f */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        /* 0x60 - 0x6f */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        /* 0x70 - 0x7f */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        /* 0x80 */        0x07,
    };
    Assert(beaReg <= 0x80);
    Assert(RegTable[beaReg] != 0xff);
    return RegTable[beaReg];
}


LX_API void LxReportError( LxResult lr )
{
    LxWarning("Error: %d\n", lr);
}

LX_API std::string LxFilePath( const char *fullPath )
{
    std::string path(fullPath);
    uint pos = path.find_last_of("\\");
    if (pos == std::string::npos) {
        pos = path.find_last_of("/");
    }
    if (pos == std::string::npos) {
        LxWarning("LxFilePath(): %s\n", fullPath);
        return "";
    }
    return path.substr(0, pos+1);
}

LX_API std::string LxFileName( const char *fullPath )
{
    std::string path(fullPath);
    uint pos = path.find_last_of("\\");
    if (pos == std::string::npos) {
        pos = path.find_last_of("/");
    }
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(pos+1, path.size());
}

LX_API uint LxNNStrLen( LPCWSTR ptr )
{
    uint len = 0;
    LPCWSTR p = ptr;
    while (*p != L'\0') {
        uint l = (wcslen(p) + 1);
        len += l;
        p += l;
    }
    return len*sizeof(wchar_t);
}

LX_API uint LxNNStrLen( LPCSTR ptr )
{
    uint len = 0;
    LPCSTR p = ptr;
    while (*p != '\0') {
        uint l = strlen(p) + 1;
        len += l;
        p += l;
    }
    return len;
}

LX_API void LxRun(int argc, LPSTR argv[])
{
	V( LxEmulator.Initialize() );
    V( LxEmulator.LoadModule(argc, argv) );
    LxEmulator.Run();
}

LX_API void LxRun( int argc, LPWSTR argv[] )
{
    const int BufSize = 4096;

    LPSTR *args = new LPSTR[argc];
    for (int i = 0; i < argc; i++) {
        LPSTR arg = new CHAR[BufSize];
        ZeroMemory(arg, BufSize);
        if (wcslen(argv[i]) >= BufSize) {
            LxFatal("Fuck Unicode\n");
        }
        LxWideToByte(argv[i], arg, BufSize);
        args[i] = arg;
    }
    
    V( LxEmulator.Initialize() );
    V( LxEmulator.LoadModule(argc, args) );
    LxEmulator.Run();

    for (int i = 0; i < argc; i++) {
        SAFE_DELETE_ARRAY(args[i]);
    }
    SAFE_DELETE_ARRAY(args);
}

LX_API const std::string &LxGetRuntimeDirectory()
{
    static std::string runtimeDirectory;

    if (!runtimeDirectory.empty()) return runtimeDirectory;

    char    pathBuf[MAX_PATH];
    HMODULE hModule = GetModuleHandle(NULL);
    uint    pathLength = GetModuleFileNameA(hModule, pathBuf, MAX_PATH);

    if (pathLength && (pathLength < sizeof(pathBuf))) {
        std::string  runtimePath(pathBuf, pathLength);
        std::string::size_type lastSlash = runtimePath.rfind('\\');
        if (lastSlash != std::string::npos) {
            runtimeDirectory = runtimePath.substr(0, lastSlash + 1);
        } else {
            LxWarning("No slash in runtime path? (%s)\n", runtimePath.c_str());
        }
    } else {
        LxWarning("Couldn't find funtime path(len=%d, err=0x%x)\n", pathLength, GetLastError());
    }
    return runtimeDirectory;
}


LX_API std::string LxGetModuleDirectory( HMODULE hModule )
{
    char        pathBuf[MAX_PATH];
    uint        pathLength = GetModuleFileNameA(hModule, pathBuf, MAX_PATH);
    std::string runtimeDirectory;

    if (pathLength && (pathLength < sizeof(pathBuf))) {
        std::string  runtimePath(pathBuf, pathLength);
        std::string::size_type lastSlash = runtimePath.rfind('\\');
        if (lastSlash != std::string::npos) {
            runtimeDirectory = runtimePath.substr(0, lastSlash + 1);
        } else {
            LxWarning("No slash in runtime path? (%s)\n", runtimePath.c_str());
        }
    } else {
        LxWarning("Couldn't find funtime path(len=%d, err=0x%x)\n", pathLength, GetLastError());
    }
    return runtimeDirectory;

}

LX_API std::string LxModulePath( const char *name )
{
    HMODULE hModule = LoadLibraryA(name);
    B( hModule != NULL );
    char filePath[MAX_PATH];
    B( GetModuleFileNameA(hModule, filePath, MAX_PATH) );
    return std::string(filePath);
}

LX_API void LxReset()
{
    LxFatal("don't use this\n");
    LxEmulator.Reset();
}

extern "C" LX_API bool LxDecodePInvoke( byte data[], Instruction *inst, u32 eip /*= 0*/ )
{
    return LxDecode(data, inst, eip);
}

extern "C" LX_API u32 LxVersion()
{
    return LOCHSEMU_VERSION;
}

LX_API bool LxRunProcess( LPCSTR exeToRun, LPCSTR params )
{
    int r = (int) ShellExecuteA(0, "open", exeToRun, params, NULL, SW_SHOW);
    return r >= 32;
}

END_NAMESPACE_LOCHSEMU()