// LochsDbg.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "LochsDbg.h"

#include "processor.h"
#include "debugger.h"
#include "diff.h"
#include "statistics.h"

PluginHandle    g_handle;
Config          g_config;


LOCHSDBG_API bool LochsEmu_Plugin_Initialize( const LochsEmuInterface *lochsemu, PluginInfo *info )
{
    g_handle = lochsemu->Handle;
    strcpy_s(info->Name, sizeof(info->Name), "LochsDbg");

    std::string cfgFile = LxGetModuleDirectory(g_module) + "lochsdbg.ini";
    g_config.Initialize(cfgFile.c_str());

    if ( !g_config.GetInt("General", "Enabled", 1) ) {
        LxInfo("LochsDbg is disabled\n");
        return false;
    }

    LxDebugger.Initialize();
    LxDiff.Initialize();
    LxStatistics.Initialize();

    return true;
}

LOCHSDBG_API void LochsEmu_Processor_PreExecute( Processor *cpu, const Instruction *inst )
{
    LxDiff.OnProcessorPreExecute(cpu, inst);
    LxDebugger.PreExecute(cpu, inst);
    LxStatistics.OnProcessorPreExecute(cpu, inst);
}

LOCHSDBG_API void LochsEmu_Processor_PostExecute( Processor *cpu, const Instruction *inst )
{
    LxDebugger.PostExecute(cpu, inst);
    LxDiff.OnProcessorPostExecute(cpu, inst);
}

LOCHSDBG_API void LochsEmu_Processor_MemRead(const Processor *cpu, u32 address, u32 nBytes, cpbyte data)
{
    LxDebugger.MemRead(cpu, address, nBytes, data);
}

LOCHSDBG_API void LochsEmu_Processor_MemWrite( const Processor *cpu, u32 address, u32 nBytes, cpbyte data )
{
    LxDebugger.MemWrite(cpu, address, nBytes, data);
}

LOCHSDBG_API void LochsEmu_Process_PreLoad( PeLoader *loader )
{
    LxDiff.OnProcessPreLoad(loader);
}

LOCHSDBG_API void LochsEmu_Process_PreRun( const Process *proc, Processor *cpu )
{
    LxDebugger.ProcessPreRun(proc, cpu);
    LxDiff.OnProcessPreRun(proc, cpu);
    LxStatistics.OnProcessPreRun(proc, cpu);
}


LOCHSDBG_API void LochsEmu_Process_PostRun( const Process *proc )
{
    LxStatistics.OnProcessPostRun(proc);
}

WORD ChangeConsoleMode( WORD mode )
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

void StdOut( const char *fmt, ... )
{
    WORD wOldMode = ChangeConsoleMode(FOREGROUND_GREEN);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    ChangeConsoleMode(wOldMode);
}

void StdDumpDark( const char *fmt, ... )
{
    WORD wOldMode = ChangeConsoleMode(FOREGROUND_BLUE|FOREGROUND_GREEN);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    ChangeConsoleMode(wOldMode);
}

void StdDumpLight( const char *fmt, ... )
{
    WORD wOldMode = ChangeConsoleMode(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    ChangeConsoleMode(wOldMode);
}

void StdError( const char *fmt, ... )
{
    WORD wOldMode = ChangeConsoleMode(FOREGROUND_RED|FOREGROUND_INTENSITY);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    ChangeConsoleMode(wOldMode);
}

