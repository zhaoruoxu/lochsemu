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
    ~AutoBreak();

    void    Initialize() override;
    void    OnPreExecute(PreExecuteEvent &event, bool firstTime) override;
    void    OnProcessPostLoad(ProcessPostLoadEvent &event, bool firstTime) override;
    void    Serialize(Json::Value &root) const override;
    void    Deserialize(Json::Value &root) override;
private:
    bool    IsExit(PreExecuteEvent &event);

private:
    bool    m_breakOnEntry;
    bool    m_breakOnMainModuleEntry;
    bool    m_fakeMessageEnd;
    bool    m_skipDllEntries;
    //bool    m_taintOriginalState;
    bool    m_tracerOriginalState;
    bool    m_mainEntryFound;

    ProDebugger *   m_debugger;
    //TaintEngine *   m_taint;
    ProTracer *     m_tracer;
    Disassembler *  m_disasm;

};


 
#endif // __PROPHET_PLUGIN_AUTOBREAK_H__