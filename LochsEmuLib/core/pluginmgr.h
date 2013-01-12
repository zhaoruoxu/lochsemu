#pragma once

#ifndef __CORE_PLUGINMGR_H__
#define __CORE_PLUGINMGR_H__

#include "lochsemu.h"
#include "pluginapi.h"

BEGIN_NAMESPACE_LOCHSEMU()

struct LoadedPluginInfo {
    HMODULE         Handle;
    PluginInfo      Info;
    /*
     * Callbacks
     */
    LochsEmu_Plugin_Initialize      Init;
    LochsEmu_Plugin_Cleanup         Cleanup;
    LochsEmu_Processor_PreExecute   ProcessorPreExecute;
    LochsEmu_Processor_PostExecute  ProcessorPostExecute;
    LochsEmu_Processor_MemRead      ProcessorMemRead;
    LochsEmu_Processor_MemWrite     ProcessorMemWrite;
    LochsEmu_Process_PreRun         ProcessPreRun;
    LochsEmu_Process_PostRun        ProcessPostRun;
    LochsEmu_Process_PreLoad        ProcessPreLoad;
    LochsEmu_Process_PostLoad       ProcessPostLoad;
    LochsEmu_Winapi_PreCall         WinapiPreCall;
    LochsEmu_Winapi_PostCall        WinapiPostCall;

    LoadedPluginInfo() {
        Handle                      = (HMODULE) 0;
        Init                        = NULL;
        Cleanup                     = NULL;
        ProcessorPreExecute         = NULL;
        ProcessorPostExecute        = NULL;
        ProcessorMemRead            = NULL;
        ProcessorMemWrite           = NULL;
        ProcessPreRun               = NULL;
        ProcessPostRun              = NULL;
        ProcessPreLoad              = NULL;
        ProcessPostLoad             = NULL;
        WinapiPreCall               = NULL;
        WinapiPostCall              = NULL;
    }
};

class PluginManager {
public:
    PluginManager();
    virtual ~PluginManager();

    LxResult        Initialize();
    LxResult OnExit                 (void);
    LxResult OnProcessorPreExecute  (Processor *cpu, const Instruction *inst);
    LxResult OnProcessorPostExecute (Processor *cpu, const Instruction *inst);
    LxResult OnProcessorMemRead     (const Processor *cpu, u32 addr, u32 nBytes, cpbyte data);
    LxResult OnProcessorMemWrite    (const Processor *cpu, u32 addr, u32 nBytes, cpbyte data);
    LxResult OnProcessPreRun        (const Process *proc, Processor *cpu);
    LxResult OnProcessPostRun       (const Process *proc);
    LxResult OnProcessPreLoad       (PeLoader *loader);
    LxResult OnProcessPostLoad      (PeLoader *loader);
    LxResult OnWinapiPreCall        (Processor *cpu, uint apiIndex);
    LxResult OnWinapiPostCall       (Processor *cpu, uint apiIndex);

private:
    bool            FindPluginDirectory();
    LxResult        LoadPlugins();
    void            LoadAPIAddrs(LoadedPluginInfo *plugin);
    bool            CheckPlugin(const LoadedPluginInfo &plugin);

private:
    std::string         m_pluginDirectory;

    typedef std::vector<LoadedPluginInfo>            PluginTable;
    typedef std::vector<LoadedPluginInfo>::iterator  PluginTableIterator;
    PluginTable         m_plugins;
    uint                m_numPlugins;
    bool                m_enablePlugins;

};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_PLUGINMGR_H__
