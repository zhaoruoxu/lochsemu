#include "stdafx.h"
#include "plugin.h"
#include "engine.h"
#include "event.h"

Plugin::Plugin(const std::string name, uint ovd)
    : m_name(name), m_enabled(true), m_ovdFlag(ovd)
{
    m_manager = g_engine.GetPluginManager();
    m_manager->RegisterPlugin(this);
}

void Plugin::Serialize( Json::Value &root ) const 
{
    root["enabled"] = m_enabled;
}

void Plugin::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();
}

ProEngine * Plugin::GetEngine()
{
    return m_manager->GetEngine();
}

const ProEngine * Plugin::GetEngine() const
{
    return m_manager->GetEngine();
}

ProPluginManager::ProPluginManager(ProEngine *engine)
    : m_engine(engine)
{
    for (int i = 0; i < MaxPlugins; i++)
        m_plugins[i] = NULL;
    m_enabled = true;
    m_totalPlugins = 0;
}

ProPluginManager::~ProPluginManager()
{

}

void ProPluginManager::Initialize()
{
    for (int i = 0; i < m_totalPlugins; i++) 
        m_plugins[i]->Initialize();
}

void ProPluginManager::RegisterPlugin( Plugin *plugin )
{
    //m_plugins.push_back(plugin);
    if (m_totalPlugins >= MaxPlugins) {
        LxFatal("Too many plugins!\n");
    }
    m_plugins[m_totalPlugins++] = plugin;
    LxInfo("Prophet plugin registered: %s\n", plugin->GetName().c_str());
}

void ProPluginManager::OnPreExecute( PreExecuteEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PreExecute)) 
            p->OnPreExecute(event, firstTime);
    }
}

void ProPluginManager::OnPostExecute( PostExecuteEvent &event, bool firstTime)
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PostExecute))
            p->OnPostExecute(event, firstTime);
    }
}

void ProPluginManager::OnMemRead( MemReadEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemRead))
            p->OnMemRead(event, firstTime);
    }
}

void ProPluginManager::OnMemWrite( MemWriteEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemWrite))
            p->OnMemWrite(event, firstTime);
    }
}

void ProPluginManager::OnProcessPreRun( ProcessPreRunEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreRun))
            p->OnProcessPreRun(event, firstTime);
    }
}

void ProPluginManager::OnProcessPostRun( ProcessPostRunEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostRun))
            p->OnProcessPostRun(event, firstTime);
    }
}

void ProPluginManager::OnProcessPreLoad( ProcessPreLoadEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreLoad))
            p->OnProcessPreLoad(event, firstTime);
    }
}

void ProPluginManager::OnProcessPostLoad( ProcessPostLoadEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostLoad))
            p->OnProcessPostLoad(event, firstTime);
    }
}

void ProPluginManager::OnWinapiPreCall( WinapiPreCallEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPreCall))
            p->OnWinapiPreCall(event, firstTime);
    }
}

void ProPluginManager::OnWinapiPostCall( WinapiPostCallEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPostCall))
            p->OnWinapiPostCall(event, firstTime);
    }
}

void ProPluginManager::Serialize( Json::Value &root ) const 
{
    root["enabled"] = m_enabled;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *plugin = m_plugins[i];
        Json::Value pluginRoot;
        plugin->Serialize(pluginRoot);
        root[plugin->GetName()] = pluginRoot;
    }
}

void ProPluginManager::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();
    if (root.isNull()) return;

    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *plugin = m_plugins[i];
        Json::Value pluginRoot = root[plugin->GetName()];
        if (pluginRoot.isNull()) continue;
        plugin->Deserialize(pluginRoot);
    }
}
