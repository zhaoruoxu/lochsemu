#include "stdafx.h"
#include "pluginmgr.h"
#include "diriter.h"
#include "config.h"

BEGIN_NAMESPACE_LOCHSEMU()


PluginManager::PluginManager()
{

}

PluginManager::~PluginManager()
{
    for (uint i = 0; i < m_plugins.size(); i++) {
        FreeLibrary(m_plugins[i].Handle);
    }
}

bool PluginManager::FindPluginDirectory()
{
    std::string runtimedirectory = LxGetRuntimeDirectory();
    if (!runtimedirectory.empty()) {
        m_pluginDirectory = runtimedirectory + "plugins\\";
        return true;
    }
    return false;
}

LxResult PluginManager::Initialize()
{
    m_enablePlugins = LxConfig.GetInt("Emulator", "EnablePlugins", 1) != 0;

    if (!m_enablePlugins) {
        LxInfo("Plugin is disabled\n");
        RET_SUCCESS();
    }
    if (FindPluginDirectory()) {
        LxInfo("Plugin directory: %s\n", m_pluginDirectory.c_str());
    }
    V( LoadPlugins() );
    RET_SUCCESS();
}

LxResult PluginManager::LoadPlugins()
{
    m_plugins.reserve(4);
    DirectoryIterator   iter(m_pluginDirectory.c_str(), "*.dll");

    for (; !iter.Done(); iter.Next()) {
        std::string path = iter.GetFullPath();
        LxInfo("Checking plugin %s\n", path.c_str());

        LoadedPluginInfo    plugin;
        LochsEmuInterface   lochsemu;

        lochsemu.Handle     = (PluginHandle)    m_plugins.size();
        lochsemu.Emulator   = &LxEmulator;
        plugin.Handle       = LoadLibraryA(path.c_str());

        if (!plugin.Handle) {
            LxWarning("Couldn't load plugin %s\n", path.c_str());
        }

        plugin.Init         = (LochsEmu_Plugin_Initialize) GetProcAddress(plugin.Handle, "LochsEmu_Plugin_Initialize");
        if (plugin.Init) {
            // try calling Init routine
            bool initOkay   = SafeCallPluginInit(&plugin, &lochsemu);

            if (!CheckPlugin(plugin)) continue;

            if (initOkay) {
                LoadAPIAddrs(&plugin);
                LxInfo("Plugin %s successfully loaded\n", path.c_str());
                LxInfo("Plugin %s, %d\n", plugin.Info.Name, lochsemu.Handle);
                m_plugins.push_back(plugin);
            } else {
                LxWarning("Plugin %s not initialized\n", path.c_str());
            }
        } else {
            LxWarning("Invalid plugin %s\n", path.c_str());
            FreeLibrary(plugin.Handle);
        }
    }
    LxInfo("%d plugins loaded\n", m_plugins.size());
    m_numPlugins = m_plugins.size();
    RET_SUCCESS();
}


void PluginManager::LoadAPIAddrs( LoadedPluginInfo *plugin )
{
    plugin->Cleanup = (LochsEmu_Plugin_Cleanup)
        GetProcAddress(plugin->Handle, "LochsEmu_Plugin_Cleanup");
    plugin->ProcessorPreExecute = (LochsEmu_Processor_PreExecute) 
        GetProcAddress(plugin->Handle, "LochsEmu_Processor_PreExecute");
    plugin->ProcessorPostExecute = (LochsEmu_Processor_PostExecute)
        GetProcAddress(plugin->Handle, "LochsEmu_Processor_PostExecute");
    plugin->ProcessorMemRead = (LochsEmu_Processor_MemRead)
        GetProcAddress(plugin->Handle, "LochsEmu_Processor_MemRead");
    plugin->ProcessorMemWrite = (LochsEmu_Processor_MemWrite)
        GetProcAddress(plugin->Handle, "LochsEmu_Processor_MemWrite");
    plugin->ProcessPreRun = (LochsEmu_Process_PreRun)
        GetProcAddress(plugin->Handle, "LochsEmu_Process_PreRun");
    plugin->ProcessPostRun = (LochsEmu_Process_PostRun)
        GetProcAddress(plugin->Handle, "LochsEmu_Process_PostRun");
    plugin->ProcessPreLoad = (LochsEmu_Process_PreLoad)
        GetProcAddress(plugin->Handle, "LochsEmu_Process_PreLoad");
    plugin->WinapiPreCall = (LochsEmu_Winapi_PreCall)
        GetProcAddress(plugin->Handle, "LochsEmu_Winapi_PreCall");
    plugin->WinapiPostCall = (LochsEmu_Winapi_PostCall)
        GetProcAddress(plugin->Handle, "LochsEmu_Winapi_PostCall");
}


bool PluginManager::CheckPlugin( const LoadedPluginInfo &plugin )
{
    PluginTableIterator iter = m_plugins.begin();
    for (; iter != m_plugins.end(); iter++) {
        if (strlen(plugin.Info.Name) == 0) {
            LxWarning("Plugin has no name: handle=%x\n", plugin.Handle);
            return false;
        }
        if (!stricmp(plugin.Info.Name, iter->Info.Name)) {
            LxWarning("Plugin %s already loaded\n", iter->Info.Name);
            return false;
        }
    }
    return true;
}


bool PluginManager::SafeCallPluginInit(LoadedPluginInfo *plugin, 
                                       const LochsEmuInterface *lochsemu )
{
    // may wanna enable SEH here; preventing any bug in plugin crashing the core
    return plugin->Init(lochsemu, &plugin->Info);
}


void PluginManager::SafeCallPluginCleanup(const LoadedPluginInfo &plugin)
{
    plugin.Cleanup();
}



END_NAMESPACE_LOCHSEMU()