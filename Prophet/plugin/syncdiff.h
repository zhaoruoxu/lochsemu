#pragma once
 
#ifndef __PROPHET_PLUGIN_SYNCDIFF_H__
#define __PROPHET_PLUGIN_SYNCDIFF_H__
 
#include "plugin.h"
#include "engine.h"

class SyncDiff : public Plugin {
public:
    SyncDiff(ProPluginManager *manager);

    void        Initialize() override;
    void        OnPreExecute(PreExecuteEvent &event, bool firstTime) override;
    void        OnPostExecute(PostExecuteEvent &event, bool firstTime) override;
    void        OnProcessPreRun(ProcessPreRunEvent &event, bool firstTime) override;
    void        OnProcessPostLoad(ProcessPostLoadEvent &event, bool firstTime) override;

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;
private:
    void        OverrideContext(Processor *cpu);
    bool        CompareContext(const Processor *cpu, const CONTEXT *ctx);
private:
    ProDebugger *   m_debugger;
    RefProcess *    m_refProc;
    PROCESS_INFORMATION *   m_pi;
    DEBUG_EVENT *   m_event;

    bool            m_synced;
    u32             m_startAddr;
};
 
#endif // __PROPHET_PLUGIN_SYNCDIFF_H__