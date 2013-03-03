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

void APluginManager::OnPreExecute( PreExecuteEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PreExecute)) 
            p->OnPreExecute(event, firstTime);
    }
}

void APluginManager::OnPostExecute( PostExecuteEvent &event, bool firstTime)
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PostExecute))
            p->OnPostExecute(event, firstTime);
    }
}

void APluginManager::OnMemRead( MemReadEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemRead))
            p->OnMemRead(event, firstTime);
    }
}

void APluginManager::OnMemWrite( MemWriteEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemWrite))
            p->OnMemWrite(event, firstTime);
    }
}

void APluginManager::OnProcessPreRun( ProcessPreRunEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreRun))
            p->OnProcessPreRun(event, firstTime);
    }
}

void APluginManager::OnProcessPostRun( ProcessPostRunEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostRun))
            p->OnProcessPostRun(event, firstTime);
    }
}

void APluginManager::OnProcessPreLoad( ProcessPreLoadEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreLoad))
            p->OnProcessPreLoad(event, firstTime);
    }
}

void APluginManager::OnProcessPostLoad( ProcessPostLoadEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostLoad))
            p->OnProcessPostLoad(event, firstTime);
    }
}

void APluginManager::OnWinapiPreCall( WinapiPreCallEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPreCall))
            p->OnWinapiPreCall(event, firstTime);
    }
}

void APluginManager::OnWinapiPostCall( WinapiPostCallEvent &event, bool firstTime )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPostCall))
            p->OnWinapiPostCall(event, firstTime);
    }
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
    //Json::Value pluginsRoot = root["plugins"];
    if (root.isNull()) return;

    for (auto &plugin : m_plugins) {
        Json::Value pluginRoot = root[plugin->GetName()];
        if (pluginRoot.isNull()) continue;
        plugin->Deserialize(pluginRoot);
    }
}
