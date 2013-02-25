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

static const uint ArietisVersion = 0x20130109;

using namespace LochsEmu;

ARIETIS_API bool LochsEmu_Plugin_Initialize         (const LochsEmuInterface *lochsemu, PluginInfo *info); 
ARIETIS_API void LochsEmu_Winapi_PreCall            (Processor *cpu, uint apiIndex);
ARIETIS_API void LochsEmu_Winapi_PostCall           (Processor *cpu, uint apiIndex);
ARIETIS_API void LochsEmu_Processor_PreExecute(Processor *cpu, const Instruction *inst);
ARIETIS_API void LochsEmu_Processor_PostExecute(Processor *cpu, const Instruction *inst);
ARIETIS_API void LochsEmu_Process_PreRun            (const Process *proc, Processor *cpu);
ARIETIS_API void LochsEmu_Process_PostLoad           (PeLoader *loader);

typedef Json::Value     AArchive;

class AEngine;
class ADebugger;
struct InstContext;
struct TraceContext;
class ATracer;
class Taint;
class TaintEngine;

extern HMODULE          g_module;
extern Config           g_config;
extern AEngine          g_engine;
extern Semaphore        g_guiSem;
extern AArchive         g_archive;

#endif // __ARIETIS_ARIETIS_H__

