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
        WinapiPreCall               = NULL;
        WinapiPostCall              = NULL;
    }
};

class PluginManager {
public:
    PluginManager();
    virtual ~PluginManager();

    LxResult        Initialize();
    INLINE LxResult OnExit                  (void);
    INLINE LxResult OnProcessorPreExecute   (Processor *cpu, const Instruction *inst);
    INLINE LxResult OnProcessorPostExecute  (Processor *cpu, const Instruction *inst);
    INLINE LxResult OnProcessorMemRead      (const Processor *cpu, u32 addr, u32 nBytes, cpbyte data);
    INLINE LxResult OnProcessorMemWrite     (const Processor *cpu, u32 addr, u32 nBytes, cpbyte data);
    INLINE LxResult OnProcessPreRun         (const Process *proc, Processor *cpu);
    INLINE LxResult OnProcessPostRun        (const Process *proc);
    INLINE LxResult OnProcessPreLoad        (PeLoader *loader);
    INLINE LxResult OnWinapiPreCall         (Processor *cpu, uint apiIndex);
    INLINE LxResult OnWinapiPostCall        (Processor *cpu, uint apiIndex);

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

INLINE LxResult 
PluginManager::OnProcessorPreExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.ProcessorPreExecute)
            plugin.ProcessorPreExecute(cpu, inst);
    }
    RET_SUCCESS();
}

INLINE LxResult 
PluginManager::OnProcessorPostExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.ProcessorPostExecute)
            plugin.ProcessorPostExecute(cpu, inst);
    }
    RET_SUCCESS();
}

INLINE LxResult 
PluginManager::OnProcessorMemRead( const Processor *cpu, u32 addr, u32 nBytes, cpbyte data )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.ProcessorMemRead)
            plugin.ProcessorMemRead(cpu, addr, nBytes, data);
    }
    RET_SUCCESS();
}

INLINE LxResult 
PluginManager::OnProcessorMemWrite( const Processor *cpu, u32 addr, u32 nBytes, cpbyte data)
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.ProcessorMemWrite)
            plugin.ProcessorMemWrite(cpu, addr, nBytes, data);
    }
    RET_SUCCESS();
}

INLINE LxResult 
PluginManager::OnExit( void )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.Cleanup)
            plugin.Cleanup();
    }
    RET_SUCCESS();
}

INLINE LxResult 
PluginManager::OnProcessPreRun( const Process *proc, Processor *cpu )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.ProcessPreRun)
            plugin.ProcessPreRun(proc, cpu);
    }
    RET_SUCCESS();
}

INLINE LxResult 
PluginManager::OnProcessPostRun( const Process *proc )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.ProcessPostRun)
            plugin.ProcessPostRun(proc);
    }
    RET_SUCCESS();
}

INLINE LxResult 
PluginManager::OnProcessPreLoad( PeLoader *loader )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.ProcessPreLoad)
            plugin.ProcessPreLoad(loader);
    }
    RET_SUCCESS();
}


INLINE LxResult 
PluginManager::OnWinapiPreCall( Processor *cpu, uint apiIndex )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.WinapiPreCall)
            plugin.WinapiPreCall(cpu, apiIndex);
    }
    RET_SUCCESS();
}

INLINE LxResult 
PluginManager::OnWinapiPostCall( Processor *cpu, uint apiIndex )
{
    if (!m_enablePlugins) RET_SUCCESS();
    for (auto plugin : m_plugins) {
        if (plugin.WinapiPostCall)
            plugin.WinapiPostCall(cpu, apiIndex);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_PLUGINMGR_H__
