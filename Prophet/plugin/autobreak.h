#pragma once
 
#ifndef __PROPHET_PLUGIN_AUTOBREAK_H__
#define __PROPHET_PLUGIN_AUTOBREAK_H__
 
#include "plugin.h"
#include "engine.h"
#include "event.h"

#include "processor.h"

class AutoBreak : public Plugin {
public:
    AutoBreak(ProPluginManager *manager);

    void    Initialize() override ;
    void    OnProcessPostLoad(ProcessPostLoadEvent &event, bool firstTime) override;
    void    OnProcessPreRun(ProcessPreRunEvent &event, bool firstTime) override;
    void    Serialize(Json::Value &root) const override;
    void    Deserialize(Json::Value &root) override;

private:
    bool    m_breakOnMainModuleEntry;
    bool    m_skipDllEntries;
    bool    m_taintOriginalState;
    bool    m_tracerOriginalState;

    ProDebugger *   m_debugger;
    TaintEngine *   m_taint;
    ProTracer *     m_tracer;
};


 
#endif // __PROPHET_PLUGIN_AUTOBREAK_H__