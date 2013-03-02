#include "stdafx.h"
#include "plugin.h"
#include "engine.h"

Plugin::Plugin(const std::string name)
    : m_name(name), m_enabled(true)
{
    m_manager = g_engine.GetPluginManager();
    m_manager->RegisterPlugin(this);
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
        if (p->IsEnabled()) 
            p->OnPreExecute(cpu, inst);
    }
}

void APluginManager::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnPostExecute(cpu, inst);
    }
}

void APluginManager::OnMemRead( const Processor *cpu, u32 addr, u32 nbytes, cpbyte data )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnMemRead(cpu, addr, nbytes, data);
    }
}

void APluginManager::OnMemWrite( const Processor *cpu, u32 addr, u32 nbytes, cpbyte data )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnMemWrite(cpu, addr, nbytes, data);
    }
}

void APluginManager::OnProcessPreRun( const Process *proc, const Processor *cpu )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnProcessPreRun(proc, cpu);
    }
}

void APluginManager::OnProcessPostRun( const Process *proc )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnProcessPostRun(proc);
    }
}

void APluginManager::OnProcessPreLoad( const PeLoader *loader )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnProcessPreLoad(loader);
    }
}

void APluginManager::OnProcessPostLoad( const PeLoader *loader )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnProcessPostLoad(loader);
    }
}

void APluginManager::OnWinapiPreCall( Processor *cpu, uint apiIndex )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnWinapiPreCall(cpu, apiIndex);
    }
}

void APluginManager::OnWinapiPostCall( Processor *cpu, uint apiIndex )
{
    for (auto p : m_plugins) {
        if (p->IsEnabled())
            p->OnWinapiPostCall(cpu, apiIndex);
    }
}
