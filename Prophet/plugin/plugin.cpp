#include "stdafx.h"
#include "plugin.h"
#include "engine.h"

#include "advdbg.h"
#include "autobreak.h"
//#include "taint_directive.h"
//#include "vulnerability_detector.h"
#include "syncdiff.h"
#include "remotediff.h"
#include "context_override.h"

Plugin::Plugin(ProPluginManager *manager, bool initialEnable, const std::string name)
    : m_manager(manager), m_name(name), m_enabled(initialEnable)
{
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
        m_plugins[i]    = NULL;
    m_enabled           = true;
    m_totalPlugins      = 0;
}

ProPluginManager::~ProPluginManager()
{
    for (int i = 0; i < m_totalPlugins; i++) {
        SAFE_DELETE(m_plugins[i]);
    }
}

void ProPluginManager::Initialize()
{
    RegisterPlugin(new ContextOverride(this));
    RegisterPlugin(new AdvancedDebugger(this));
    RegisterPlugin(new AutoBreak(this));
    //RegisterPlugin(new TaintDirective(this));
    //RegisterPlugin(new VulnerabilityDetector(this));
    RegisterPlugin(new SyncDiff(this));
    RegisterPlugin(new RemoteDiff(this));

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
        if (p->IsEnabled()) 
            p->OnPreExecute(event, firstTime);
    }
}

void ProPluginManager::OnPostExecute( PostExecuteEvent &event, bool firstTime)
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnPostExecute(event, firstTime);
    }
}

void ProPluginManager::OnMemRead( MemReadEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnMemRead(event, firstTime);
    }
}

void ProPluginManager::OnMemWrite( MemWriteEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnMemWrite(event, firstTime);
    }
}

void ProPluginManager::OnProcessPreRun( ProcessPreRunEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnProcessPreRun(event, firstTime);
    }
}

void ProPluginManager::OnProcessPostRun( ProcessPostRunEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnProcessPostRun(event, firstTime);
    }
}

void ProPluginManager::OnProcessPreLoad( ProcessPreLoadEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnProcessPreLoad(event, firstTime);
    }
}

void ProPluginManager::OnProcessPostLoad( ProcessPostLoadEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnProcessPostLoad(event, firstTime);
    }
}

void ProPluginManager::OnWinapiPreCall( WinapiPreCallEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnWinapiPreCall(event, firstTime);
    }
}

void ProPluginManager::OnWinapiPostCall( WinapiPostCallEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnWinapiPostCall(event, firstTime);
    }
}

void ProPluginManager::OnThreadCreate( ThreadCreateEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnThreadCreate(event, firstTime);
    }
}

void ProPluginManager::OnThreadExit( ThreadExitEvent &event, bool firstTime )
{
    if (!m_enabled) return;
    for (int i = 0; i < m_totalPlugins; i++) {
        Plugin *p = m_plugins[i];
        if (p->IsEnabled())
            p->OnThreadExit(event, firstTime);
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
