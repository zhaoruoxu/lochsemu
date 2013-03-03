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

APluginManager::APluginManager(AEngine *engine)
    : m_engine(engine)
{

}

APluginManager::~APluginManager()
{

}

void APluginManager::Initialize()
{
    for (auto p : m_plugins) 
        p->Initialize();
}

void APluginManager::RegisterPlugin( Plugin *plugin )
{
    m_plugins.push_back(plugin);
    LxInfo("Arietis plugin registered: %s\n", plugin->GetName().c_str());
}

void APluginManager::OnPreExecute( PreExecuteEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PreExecute)) 
            p->OnPreExecute(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnPostExecute( PostExecuteEvent &event)
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PostExecute))
            p->OnPostExecute(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnMemRead( MemReadEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemRead))
            p->OnMemRead(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnMemWrite( MemWriteEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemWrite))
            p->OnMemWrite(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnProcessPreRun( ProcessPreRunEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreRun))
            p->OnProcessPreRun(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnProcessPostRun( ProcessPostRunEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostRun))
            p->OnProcessPostRun(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnProcessPreLoad( ProcessPreLoadEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreLoad))
            p->OnProcessPreLoad(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnProcessPostLoad( ProcessPostLoadEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostLoad))
            p->OnProcessPostLoad(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnWinapiPreCall( WinapiPreCallEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPreCall))
            p->OnWinapiPreCall(event);
    }
    event.PluginInvoke();
}

void APluginManager::OnWinapiPostCall( WinapiPostCallEvent &event )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPostCall))
            p->OnWinapiPostCall(event);
    }
    event.PluginInvoke();
}

void APluginManager::Serialize( Json::Value &root ) const 
{
    for (auto &plugin : m_plugins) {
        Json::Value pluginRoot;
        plugin->Serialize(pluginRoot);
        root[plugin->GetName()] = pluginRoot;
    }
}

void APluginManager::Deserialize( Json::Value &root )
{
    Json::Value pluginsRoot = root["plugins"];
    if (pluginsRoot.isNull()) return;

    for (auto &plugin : m_plugins) {
        Json::Value pluginRoot = pluginsRoot[plugin->GetName()];
        if (pluginRoot.isNull()) continue;
        plugin->Deserialize(pluginRoot);
    }
}
