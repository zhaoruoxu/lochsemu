#include "stdafx.h"
#include "autobreak.h"
#include "emulator.h"

AutoBreak::AutoBreak(ProPluginManager *manager) 
    : Plugin(manager, true, "AutoBreak") 
{
    m_breakOnMainModuleEntry    = true;
    m_fakeMessageEnd     = true;
    m_skipDllEntries    = true;
    m_debugger          = NULL;
    m_mainEntryFound    = false;
    m_breakOnEntry      = false;
}

AutoBreak::~AutoBreak()
{
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

    if (m_mainEntryFound && m_fakeMessageEnd) {
        if (IsExit(event) && GetEngine()->GetProtocol()->GetState() == Protocol::ProcessingMessage) {
            MessageEndEvent e(this);
            GetEngine()->GetProtocol()->OnMessageEnd(e);
            GetEngine()->BreakOnNextInst("Fake message end");
        }
    }

    if (m_mainEntryFound) return;
    if (event.Cpu->GetCurrentModule() == 0) {
        // come to main module for the first time
        m_mainEntryFound = true;
        m_disasm->GetInst(event.Cpu, event.Cpu->EIP)->Desc = "Main module entry";
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
    if (m_skipDllEntries) {
        // 禁用DLL初始化时的Taint和Tracer执行
        m_taintOriginalState    = m_taint->IsEnabled();
        m_tracerOriginalState   = m_tracer->IsEnabled();
        m_taint->Enable(false);
        m_tracer->Enable(false);
    }
    if (m_breakOnEntry) {
        m_debugger->SetState(m_debugger->GetCurrentThreadId(), ProDebugger::STATE_SINGLESTEP);
    }
}

void AutoBreak::Serialize( Json::Value &root ) const 
{
    Plugin::Serialize(root);
    root["break_on_main_module_entry"]  = m_breakOnMainModuleEntry;
    root["fake_message_end"]   = m_fakeMessageEnd;
    root["skip_dll_entries"]    = m_skipDllEntries;
    root["break_on_entry"]      = m_breakOnEntry;
}

void AutoBreak::Deserialize( Json::Value &root )
{
    Plugin::Deserialize(root);
    m_breakOnMainModuleEntry = root.get("break_on_main_module_entry", 
        m_breakOnMainModuleEntry).asBool();
    m_fakeMessageEnd = root.get("fake_message_end",
        m_fakeMessageEnd).asBool();
    m_skipDllEntries = root.get("skip_dll_entries", m_skipDllEntries).asBool();
    m_breakOnEntry = root.get("break_on_entry", m_breakOnEntry).asBool();
}

bool AutoBreak::IsExit( PreExecuteEvent &event )
{
    InstPtr pinst = m_disasm->GetInst(event.Cpu, event.Cpu->GetValidEip());
    if (strcmp(pinst->TargetFuncName, "ExitProcess") == 0)
        return true;
    return false;
}


