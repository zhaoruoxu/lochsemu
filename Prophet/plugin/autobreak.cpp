#include "stdafx.h"
#include "autobreak.h"

AutoBreak::AutoBreak(ProPluginManager *manager) 
    : Plugin(manager, "AutoBreak", ProcessPreRunHandler | ProcessPostLoadHandler | PreExecuteHandler) 
{
    m_breakOnMainModuleEntry = true;
    m_skipDllEntries    = true;
    m_debugger          = NULL;
    m_mainEntryFound    = false;
}

void AutoBreak::Initialize()
{
    m_debugger  = GetEngine()->GetDebugger();
    m_taint     = GetEngine()->GetTaintEngine();
    m_tracer    = GetEngine()->GetTracer();
    m_disasm    = GetEngine()->GetDisassembler();
}


void AutoBreak::OnPreExecute( PreExecuteEvent &event, bool firstTime )
{
    if (!firstTime) return;
    if (m_mainEntryFound) return;
    if (event.Cpu->GetCurrentModule() == 0) {
        // come to main module for the first time
        m_mainEntryFound = true;
        m_disasm->GetInst(event.Cpu->EIP)->Desc = "Main module entry";
        GetEngine()->RefreshGUI();
        if (m_breakOnMainModuleEntry) {
            GetEngine()->BreakOnNextInst("Main module entry");
            LxInfo("AutoBreak: Main module at %08x\n", event.Cpu->EIP);
        }
        if (m_skipDllEntries) {
            m_taint->Enable(m_taintOriginalState);
            m_tracer->Enable(m_tracerOriginalState);
        }
    }
}


void AutoBreak::OnProcessPostLoad( ProcessPostLoadEvent &event, bool firstTime )
{
    if (firstTime) return;
    //m_debugger->SetState(m_skipDllEntries ? ProDebugger::STATE_RUNNING : ProDebugger::STATE_SINGLESTEP);
    if (m_skipDllEntries) {
        // 禁用DLL初始化时的Taint和Tracer执行
        m_debugger->SetState(ProDebugger::STATE_SINGLESTEP);
        m_taintOriginalState    = m_taint->IsEnabled();
        m_tracerOriginalState   = m_tracer->IsEnabled();
        m_taint->Enable(false);
        m_tracer->Enable(false);
    }
}

void AutoBreak::OnProcessPreRun( ProcessPreRunEvent &event, bool firstTime )
{
    if (firstTime) return;

//     m_disasm->GetInst(event.Proc->GetEntryPoint())->Desc = "Main module entry";
// 
//     if (m_breakOnMainModuleEntry) {
//         GetEngine()->BreakOnNextInst("Main module entry");
//         LxInfo("AutoBreak: Main module at %08x\n", event.Proc->GetEntryPoint());
//     }
// 
//     if (m_skipDllEntries) {
//         // 恢复Taint和Tracer执行
//         m_taint->Enable(m_taintOriginalState);
//         m_tracer->Enable(m_tracerOriginalState);
//     }
}

void AutoBreak::Serialize( Json::Value &root ) const 
{
    Plugin::Serialize(root);
    root["break_on_main_module_entry"] = m_breakOnMainModuleEntry;
    root["skip_dll_entries"] = m_skipDllEntries;
}

void AutoBreak::Deserialize( Json::Value &root )
{
    Plugin::Deserialize(root);
    m_breakOnMainModuleEntry = root.get("break_on_main_module_entry", 
        m_breakOnMainModuleEntry).asBool();
    m_skipDllEntries = root.get("skip_dll_entries", m_skipDllEntries).asBool();
}


