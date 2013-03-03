#include "stdafx.h"
#include "plugin.h"
#include "engine.h"
#include "event.h"

#include "processor.h"

class AutoBreak : public Plugin {
public:
    AutoBreak() : Plugin("AutoBreak", Func_ProcessPreRun | Func_ProcessPostLoad) 
    {
        m_breakOnMainModuleEntry = true;
        m_skipDllEntries = true;
        m_debugger = NULL;
    }

    ~AutoBreak() {}

    void    Initialize() override 
    {
        m_debugger = GetManager()->GetEngine()->GetDebugger();
    }

//     void    OnPreExecute(PreExecuteEvent &event, bool firstTime) override 
//     {
//         if (!firstTime) return;
//         if (m_skipDllEntries && !m_mainModuleEncountered) {
//             if (event.Cpu->GetCurrentModule() != 0)
//                 event.Veto();
//         }
//     }

    void    OnProcessPostLoad(ProcessPostLoadEvent &event, bool firstTime) override 
    {
        if (!firstTime) return;
        m_debugger->SetState(m_skipDllEntries ? ADebugger::STATE_RUNNING : ADebugger::STATE_SINGLESTEP);
    }

    void    OnProcessPreRun(ProcessPreRunEvent &event, bool firstTime) override
    {
        if (firstTime) return;

        if (m_breakOnMainModuleEntry) {
            m_debugger->SetState(ADebugger::STATE_SINGLESTEP);
            LxInfo("AutoBreak: Main module at %08x\n", event.Cpu->EIP);
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

    ADebugger * m_debugger;
};


static AutoBreak t;