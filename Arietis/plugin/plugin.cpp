#include "stdafx.h"
#include "plugin.h"
#include "engine.h"

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

void APluginManager::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PreExecute)) 
            p->OnPreExecute(cpu, inst);
    }
}

void APluginManager::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_PostExecute))
            p->OnPostExecute(cpu, inst);
    }
}

void APluginManager::OnMemRead( const Processor *cpu, u32 addr, u32 nbytes, cpbyte data )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemRead))
            p->OnMemRead(cpu, addr, nbytes, data);
    }
}

void APluginManager::OnMemWrite( const Processor *cpu, u32 addr, u32 nbytes, cpbyte data )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_MemWrite))
            p->OnMemWrite(cpu, addr, nbytes, data);
    }
}

void APluginManager::OnProcessPreRun( const Process *proc, const Processor *cpu )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreRun))
            p->OnProcessPreRun(proc, cpu);
    }
}

void APluginManager::OnProcessPostRun( const Process *proc )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostRun))
            p->OnProcessPostRun(proc);
    }
}

void APluginManager::OnProcessPreLoad( const PeLoader *loader )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPreLoad))
            p->OnProcessPreLoad(loader);
    }
}

void APluginManager::OnProcessPostLoad( const PeLoader *loader )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_ProcessPostLoad))
            p->OnProcessPostLoad(loader);
    }
}

void APluginManager::OnWinapiPreCall( Processor *cpu, uint apiIndex )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPreCall))
            p->OnWinapiPreCall(cpu, apiIndex);
    }
}

void APluginManager::OnWinapiPostCall( Processor *cpu, uint apiIndex )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled() && p->HasOverrideFlag(Func_WinapiPostCall))
            p->OnWinapiPostCall(cpu, apiIndex);
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
    Json::Value pluginsRoot = root["plugins"];
    if (pluginsRoot.isNull()) return;

    for (auto &plugin : m_plugins) {
        Json::Value pluginRoot = pluginsRoot[plugin->GetName()];
        if (pluginRoot.isNull()) continue;
        plugin->Deserialize(pluginRoot);
    }
}
