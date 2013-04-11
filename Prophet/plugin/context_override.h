#pragma once
 
#ifndef __PROPHET_PLUGIN_CONTEXT_OVERRIDE_H__
#define __PROPHET_PLUGIN_CONTEXT_OVERRIDE_H__
 
#include "plugin.h"
#include "engine.h"

class ContextOverride : public Plugin {
public:
    ContextOverride(ProPluginManager *manager);

    void        Initialize() override;
    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;
    void        OnPreExecute(PreExecuteEvent &event, bool firstTime) override;

private:
    void        OverrideContext(Processor *cpu);

private:
    PROCESS_INFORMATION *   m_pi;
    DEBUG_EVENT *   m_event;

    bool        m_done;

};

#endif // __PROPHET_PLUGIN_CONTEXT_OVERRIDE_H__