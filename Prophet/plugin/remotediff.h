#pragma once
 
#ifndef __PROPHET_PLUGIN_REMOTEDIFF_H__
#define __PROPHET_PLUGIN_REMOTEDIFF_H__
 
#include "plugin.h"
#include "engine.h"
#include "remotediff/pipeserver.h"

class RemoteDiff : public Plugin {
public:
    RemoteDiff(ProPluginManager *manager);

    void        Initialize() override;
    void        OnPostExecute(PostExecuteEvent &event, bool firstTime) override;
    void        OnProcessPreRun(ProcessPreRunEvent &event, bool firstTime) override;
    void        OnThreadCreate(ThreadCreateEvent &event, bool firstTime) override;
    void        OnThreadExit(ThreadExitEvent &event, bool firstTime) override;

private:
    bool        CompareContextData(PostExecuteEvent &event, ContextData &data);
private:
    bool            m_synced;
    ProDebugger *   m_debugger;
    PipeServer      m_server;
};
 
#endif // __PROPHET_PLUGIN_REMOTEDIFF_H__