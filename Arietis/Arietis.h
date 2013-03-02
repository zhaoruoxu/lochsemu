#pragma once
 
#ifndef __ARIETIS_ARIETIS_H__
#define __ARIETIS_ARIETIS_H__
 
#include "config.h"

#ifdef ARIETIS_EXPORTS
#define ARIETIS_API extern "C" __declspec(dllexport)
#else
#define ARIETIS_API extern "C" __declspec(dllimport)
#endif

#include "pluginapi.h"
#include "json/json.h"

static const uint ArietisVersion = 0x20130225;

using namespace LochsEmu;

ARIETIS_API bool LochsEmu_Plugin_Initialize         (const LochsEmuInterface *lochsemu, PluginInfo *info); 
ARIETIS_API bool LochsEmu_Plugin_Cleanup            (void);
ARIETIS_API void LochsEmu_Processor_PreExecute      (Processor *cpu, const Instruction *inst);
ARIETIS_API void LochsEmu_Processor_PostExecute     (Processor *cpu, const Instruction *inst);
ARIETIS_API void LochsEmu_Processor_MemRead         (const Processor *cpu, u32 address, u32 nBytes, cpbyte data);
ARIETIS_API void LochsEmu_Processor_MemWrite        (const Processor *cpu, u32 address, u32 nBytes, cpbyte data);
ARIETIS_API void LochsEmu_Process_PreRun            (const Process *proc, Processor *cpu);
ARIETIS_API void LochsEmu_Process_PostRun           (const Process *proc);
ARIETIS_API void LochsEmu_Process_PreLoad           (PeLoader *loader);
ARIETIS_API void LochsEmu_Process_PostLoad          (PeLoader *loader);
ARIETIS_API void LochsEmu_Winapi_PreCall            (Processor *cpu, uint apiIndex);
ARIETIS_API void LochsEmu_Winapi_PostCall           (Processor *cpu, uint apiIndex);
// dbg
class ADebugger;
struct TraceContext;
class ATracer;
struct Breakpoint;

// gui

// static
struct Inst;
class Disassembler;

// taint
class Taint;
class TaintEngine;

// core
class AEngine;
struct InstContext;
struct Archive;

// plugin
class Plugin;
class APluginManager;

extern Emulator *       g_emulator;
extern HMODULE          g_module;
extern Config           g_config;
extern AEngine          g_engine;
extern Semaphore        g_guiSem;

#endif // __ARIETIS_ARIETIS_H__

