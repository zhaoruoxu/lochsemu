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

}

ProPluginManager::~ProPluginManager()
{

}

void ProPluginManager::Initialize()
{
    for (auto p : m_plugins) 
        p->Initialize();
}

void ProPluginManager::RegisterPlugin( Plugin *plugin )
{
    m_plugins.push_back(plugin);
    LxInfo("Prophet plugin registered: %s\n", plugin->GetName().c_str());
}

void ProPluginManager::OnPreExecute( PreExecuteEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PreExecute)) 
            p->OnPreExecute(event, firstTime);
    }
}

void ProPluginManager::OnPostExecute( PostExecuteEvent &event, bool firstTime)
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PostExecute))
            p->OnPostExecute(event, firstTime);
    }
}

void ProPluginManager::OnMemRead( MemReadEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemRead))
            p->OnMemRead(event, firstTime);
    }
}

void ProPluginManager::OnMemWrite( MemWriteEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemWrite))
            p->OnMemWrite(event, firstTime);
    }
}

void ProPluginManager::OnProcessPreRun( ProcessPreRunEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreRun))
            p->OnProcessPreRun(event, firstTime);
    }
}

void ProPluginManager::OnProcessPostRun( ProcessPostRunEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostRun))
            p->OnProcessPostRun(event, firstTime);
    }
}

void ProPluginManager::OnProcessPreLoad( ProcessPreLoadEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreLoad))
            p->OnProcessPreLoad(event, firstTime);
    }
}

void ProPluginManager::OnProcessPostLoad( ProcessPostLoadEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostLoad))
            p->OnProcessPostLoad(event, firstTime);
    }
}

void ProPluginManager::OnWinapiPreCall( WinapiPreCallEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPreCall))
            p->OnWinapiPreCall(event, firstTime);
    }
}

void ProPluginManager::OnWinapiPostCall( WinapiPostCallEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPostCall))
            p->OnWinapiPostCall(event, firstTime);
    }
}

void ProPluginManager::Serialize( Json::Value &root ) const 
{
    for (auto &plugin : m_plugins) {
        Json::Value pluginRoot;
        plugin->Serialize(pluginRoot);
        root[plugin->GetName()] = pluginRoot;
    }
}

void ProPluginManager::Deserialize( Json::Value &root )
{
    //Json::Value pluginsRoot = root["plugins"];
    if (root.isNull()) return;

    for (auto &plugin : m_plugins) {
        Json::Value pluginRoot = root[plugin->GetName()];
        if (pluginRoot.isNull()) continue;
        plugin->Deserialize(pluginRoot);
    }
}
