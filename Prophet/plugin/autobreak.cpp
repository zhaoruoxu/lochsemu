#include "stdafx.h"
#include "plugin.h"
#include "engine.h"
#include "event.h"

#include "processor.h"

class AutoBreak : public Plugin {
public:
    AutoBreak() : Plugin("AutoBreak", ProcessPreRunHandler | ProcessPostLoadHandler) 
    {
        m_breakOnMainModuleEntry = true;
        m_skipDllEntries = true;
        m_debugger = NULL;
    }

    ~AutoBreak() {}

    void    Initialize() override 
    {
        m_debugger  = GetEngine()->GetDebugger();
        m_taint     = GetEngine()->GetTaintEngine();
        m_tracer    = GetEngine()->GetTracer();
    }

    void    OnProcessPostLoad(ProcessPostLoadEvent &event, bool firstTime) override 
    {
        if (!firstTime) return;
        m_debugger->SetState(m_skipDllEntries ? ProDebugger::STATE_RUNNING : ProDebugger::STATE_SINGLESTEP);
        if (m_skipDllEntries) {
            // 禁用DLL初始化时的Taint和Tracer执行
            m_taintOriginalState    = m_taint->IsEnabled();
            m_tracerOriginalState   = m_tracer->IsEnabled();
            m_taint->Enable(false);
            m_tracer->Enable(false);
        }
    }

    void    OnProcessPreRun(ProcessPreRunEvent &event, bool firstTime) override
    {
        if (firstTime) return;

        if (m_breakOnMainModuleEntry) {
            m_debugger->SetState(ProDebugger::STATE_SINGLESTEP);
            LxInfo("AutoBreak: Main module at %08x\n", event.Cpu->EIP);
        }

        if (m_skipDllEntries) {
            // 恢复Taint和Tracer执行
            m_taint->Enable(m_taintOriginalState);
            m_tracer->Enable(m_tracerOriginalState);
        }
    }

    void    Serialize(Json::Value &root) const override
    {
        Plugin::Serialize(root);
        root["break_on_main_module_entry"] = m_breakOnMainModuleEntry;
        root["skip_dll_entries"] = m_skipDllEntries;
    }

    void    Deserialize(Json::Value &root) override
    {
        Plugin::Deserialize(root);
        m_breakOnMainModuleEntry = root.get("break_on_main_module_entry", 
            m_breakOnMainModuleEntry).asBool();
        m_skipDllEntries = root.get("skip_dll_entries", m_skipDllEntries).asBool();
    }

private:
    bool    m_breakOnMainModuleEntry;
    bool    m_skipDllEntries;

    bool    m_taintOriginalState;
    bool    m_tracerOriginalState;

    ProDebugger *   m_debugger;
    TaintEngine *   m_taint;
    ProTracer *     m_tracer;
};


AutoBreak t;