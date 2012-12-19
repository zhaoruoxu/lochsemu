#pragma once

#ifndef __LOCHSDBG_LOCHSDBG_H__
#define __LOCHSDBG_LOCHSDBG_H__

#include "config.h"

#ifdef LOCHSDBG_EXPORTS
#define LOCHSDBG_API extern "C" __declspec(dllexport)
#else
#define LOCHSDBG_API extern "C" __declspec(dllimport)
#endif


#include "pluginapi.h"

using namespace LochsEmu;
using std::stringstream;

/*
 * API declarations
 */
LOCHSDBG_API bool LochsEmu_Plugin_Initialize        (const LochsEmuInterface *lochsemu, PluginInfo *info);
LOCHSDBG_API void LochsEmu_Processor_PreExecute     (Processor *cpu, const Instruction *inst);
LOCHSDBG_API void LochsEmu_Processor_PostExecute    (Processor *cpu, const Instruction *inst);
LOCHSDBG_API void LochsEmu_Processor_MemRead        (const Processor *cpu, u32 address, u32 nBytes, cpbyte data);
LOCHSDBG_API void LochsEmu_Processor_MemWrite       (const Processor *cpu, u32 address, u32 nBytes, cpbyte data);
LOCHSDBG_API void LochsEmu_Process_PreLoad          (PeLoader *loader);
LOCHSDBG_API void LochsEmu_Process_PreRun           (const Process *proc, Processor *cpu);
LOCHSDBG_API void LochsEmu_Process_PostRun          (const Process *proc);


/*
 * Class Declarations
 */
class   Debugger;
class   DataStream;
class   Console;

/*
 * Utilities
 */
void    StdOut          (const char *fmt, ...);
void    StdError        (const char *fmt, ...);
void    StdDumpLight    (const char *fmt, ...);
void    StdDumpDark     (const char *fmt, ...);
WORD    ChangeConsoleMode(WORD mode);


extern HMODULE  g_module;
extern Config   g_config;

// trim from start
//static inline std::string &ltrim(std::string &s) {
//    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
//    return s;
//}
//
//// trim from end
//static inline std::string &rtrim(std::string &s) {
//    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
//    return s;
//}
//
//static inline std::string &trim(std::string &s) {
//    return ltrim(rtrim(s));
//}


#endif // __LOCHSDBG_LOCHSDBG_H__
