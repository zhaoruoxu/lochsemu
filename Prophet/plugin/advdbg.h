#pragma once
 
#ifndef __PROPHET_PLUGIN_ADVDBG_H__
#define __PROPHET_PLUGIN_ADVDBG_H__
 
#include "plugin.h"
#include "engine.h"

class AdvancedDebugger : public Plugin {
public:
    AdvancedDebugger(ProPluginManager *manager); 

    void    Initialize() override;
    void    Serialize(Json::Value &root) const override;
    void    Deserialize(Json::Value &root) override;
    void    OnPreExecute(PreExecuteEvent &event, bool firstTime) override;

private:
    void    AnalyzeWinMain(PreExecuteEvent &event);
    void    MarkWinMain(u32 addr);
private:
    ProDebugger *   m_debugger;
    Disassembler *  m_disasm;

    bool    m_breakOnWinMain;
    int     m_callsBackward;
};

 
#endif // __PROPHET_PLUGIN_ADVDBG_H__