#pragma once
 
#ifndef __PROPHET_PROPHET_H__
#define __PROPHET_PROPHET_H__
 
#include "config.h"

#ifdef PROPHET_EXPORTS
#define PROPHET_API extern "C" __declspec(dllexport)
#else
#define PROPHET_API extern "C" __declspec(dllimport)
#endif

#include "pluginapi.h"
#include "json/json.h"

static const uint ProphetVersion = 0x20130304;

using namespace LochsEmu;

PROPHET_API bool LochsEmu_Plugin_Initialize         (const LochsEmuInterface *lochsemu, PluginInfo *info); 
PROPHET_API bool LochsEmu_Plugin_Cleanup            (void);
PROPHET_API void LochsEmu_Processor_PreExecute      (Processor *cpu, const Instruction *inst);
PROPHET_API void LochsEmu_Processor_PostExecute     (Processor *cpu, const Instruction *inst);
PROPHET_API void LochsEmu_Processor_MemRead         (const Processor *cpu, u32 address, u32 nBytes, cpbyte data);
PROPHET_API void LochsEmu_Processor_MemWrite        (const Processor *cpu, u32 address, u32 nBytes, cpbyte data);
PROPHET_API void LochsEmu_Process_PreRun            (const Process *proc, Processor *cpu);
PROPHET_API void LochsEmu_Process_PostRun           (const Process *proc);
PROPHET_API void LochsEmu_Process_PreLoad           (PeLoader *loader);
PROPHET_API void LochsEmu_Process_PostLoad          (PeLoader *loader);
PROPHET_API void LochsEmu_Winapi_PreCall            (Processor *cpu, uint apiIndex);
PROPHET_API void LochsEmu_Winapi_PostCall           (Processor *cpu, uint apiIndex);
// dbg
class ProDebugger;
struct TraceContext;
class ProTracer;
struct Breakpoint;

// gui

// static
struct Inst;
class Disassembler;

// taint
class Taint;
class TaintEngine;

// core
class ProEngine;
struct InstContext;
struct Archive;

// plugin
class Plugin;
class ProPluginManager;

// event
class Event;
class PreExecuteEvent;
class PostExecuteEvent;
class MemReadEvent;
class MemWriteEvent;
class ProcessPreRunEvent;
class ProcessPostRunEvent;
class ProcessPreLoadEvent;
class ProcessPostLoadEvent;
class WinapiPreCallEvent;
class WinapiPostCallEvent;

// protocol
class Protocol;
class Message;
class MessageManager;
class FormatSyn;

extern Emulator *       g_emulator;
extern HMODULE          g_module;
extern Config           g_config;
extern ProEngine          g_engine;
extern Semaphore        g_guiSem;

#endif // __PROPHET_PROPHET_H__

