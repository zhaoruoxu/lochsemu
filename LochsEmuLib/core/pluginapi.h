#pragma once

#ifndef __CORE_PLUGINAPI_H__
#define __CORE_PLUGINAPI_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

typedef uint    PluginHandle;

struct LochsEmuInterface {
    PluginHandle    Handle;
    Emulator *      Emulator;
    // and other stuff
};

struct PluginInfo {
    char            Name[64];
    // other stuff

    PluginInfo() {
        ZeroMemory(Name, 64);
    }
};

typedef bool (* LochsEmu_Plugin_Initialize)     (const LochsEmuInterface *lochsemu, PluginInfo *info);
typedef bool (* LochsEmu_Plugin_Cleanup)        (void);
typedef void (* LochsEmu_Processor_PreExecute)  (Processor *cpu, const Instruction *inst);
typedef void (* LochsEmu_Processor_PostExecute) (Processor *cpu, const Instruction *inst);
typedef void (* LochsEmu_Processor_MemRead)     (const Processor *cpu, u32 address, u32 nBytes, cpbyte data);
typedef void (* LochsEmu_Processor_MemWrite)    (const Processor *cpu, u32 address, u32 nBytes, cpbyte data);
typedef void (* LochsEmu_Process_PreRun)        (const Process *proc, Processor *cpu);
typedef void (* LochsEmu_Process_PostRun)       (const Process *proc);
typedef void (* LochsEmu_Process_PreLoad)       (PeLoader *loader);


END_NAMESPACE_LOCHSEMU()

#endif // __CORE_PLUGINAPI_H__
