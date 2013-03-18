#pragma once
 
#ifndef __PROPHET_PLUGIN_PLUGIN_H__
#define __PROPHET_PLUGIN_PLUGIN_H__
 
#include "prophet.h"
#include "utilities.h"
#include "event.h"

class Plugin : public ISerializable {
public:
    Plugin(ProPluginManager *manager, const std::string name, uint ovd);
    virtual ~Plugin() {}

    std::string     GetName() const { return m_name; }
    bool            IsEnabled() const { return m_enabled; }
    void            Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool            HasOverrideFlag(EventHandlerFlag f) const { return (m_ovdFlag & f) != 0; }
    ProPluginManager* GetManager() { return m_manager; }
    const ProPluginManager *  GetManager() const { return m_manager; }
    ProEngine *       GetEngine();
    const ProEngine * GetEngine() const;

    void            Serialize(Json::Value &root) const override;
    void            Deserialize(Json::Value &root) override;

    virtual void    Initialize() {}
    virtual void    OnPreExecute        (PreExecuteEvent        &event, bool firstTime) {}
    virtual void    OnPostExecute       (PostExecuteEvent       &event, bool firstTime) {}
    virtual void    OnMemRead           (MemReadEvent           &event, bool firstTime) {}
    virtual void    OnMemWrite          (MemWriteEvent          &event, bool firstTime) {}
    virtual void    OnProcessPreRun     (ProcessPreRunEvent     &event, bool firstTime) {}
    virtual void    OnProcessPostRun    (ProcessPostRunEvent    &event, bool firstTime) {}
    virtual void    OnProcessPreLoad    (ProcessPreLoadEvent    &event, bool firstTime) {}
    virtual void    OnProcessPostLoad   (ProcessPostLoadEvent   &event, bool firstTime) {}
    virtual void    OnWinapiPreCall     (WinapiPreCallEvent     &event, bool firstTime) {}
    virtual void    OnWinapiPostCall    (WinapiPostCallEvent    &event, bool firstTime) {}
    
private:
    std::string     m_name;
    bool            m_enabled;
    ProPluginManager *m_manager;
    uint    m_ovdFlag;
};


class ProPluginManager : public ISerializable {
public:
    ProPluginManager(ProEngine *engine);
    ~ProPluginManager();


    void            Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool            IsEnabled() const { return m_enabled; }
    void            Initialize();
    ProEngine *         GetEngine() { return m_engine; }
    const ProEngine *   GetEngine() const { return m_engine; }

    int             GetNumPlugins() const { return m_totalPlugins; }
    Plugin *        GetPlugin(int n) { return m_plugins[n]; }

    virtual void    Serialize(Json::Value &root) const override;
    virtual void    Deserialize(Json::Value &root) override;

    void            OnPreExecute        (PreExecuteEvent        &event, bool firstTime);
    void            OnPostExecute       (PostExecuteEvent       &event, bool firstTime);
    void            OnMemRead           (MemReadEvent           &event, bool firstTime);
    void            OnMemWrite          (MemWriteEvent          &event, bool firstTime);
    void            OnProcessPreRun     (ProcessPreRunEvent     &event, bool firstTime);
    void            OnProcessPostRun    (ProcessPostRunEvent    &event, bool firstTime);
    void            OnProcessPreLoad    (ProcessPreLoadEvent    &event, bool firstTime);
    void            OnProcessPostLoad   (ProcessPostLoadEvent   &event, bool firstTime);
    void            OnWinapiPreCall     (WinapiPreCallEvent     &event, bool firstTime);
    void            OnWinapiPostCall    (WinapiPostCallEvent    &event, bool firstTime);

private:
    void            RegisterPlugin(Plugin *plugin);
private:
    //std::vector<Plugin *>   m_plugins;
    static const int    MaxPlugins = 32;
    int             m_totalPlugins;
    Plugin *        m_plugins[MaxPlugins];
    ProEngine *     m_engine;
    bool            m_enabled;
};

#endif // __PROPHET_PLUGIN_PLUGIN_H__