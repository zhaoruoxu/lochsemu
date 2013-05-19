#include "stdafx.h"
#include "advdbg.h"

AdvancedDebugger::AdvancedDebugger(ProPluginManager *manager)
    : Plugin(manager, false, "Advanced Debugger")
{
    m_breakOnWinMain    = true;
    m_callsBackward     = 0;
}

void AdvancedDebugger::Initialize()
{
    m_debugger  = GetEngine()->GetDebugger();
    m_disasm    = GetEngine()->GetDisassembler();
}

void AdvancedDebugger::Serialize( Json::Value &root ) const 
{
    Plugin::Serialize(root);
    root["break_on_winmain"] = m_breakOnWinMain;
}

void AdvancedDebugger::Deserialize( Json::Value &root )
{
    Plugin::Deserialize(root);
    m_breakOnWinMain = root.get("break_on_winmain", m_breakOnWinMain).asBool();
}

void AdvancedDebugger::OnPreExecute( PreExecuteEvent &event, bool firstTime )
{
    if (!firstTime) return;
    AnalyzeWinMain(event); 
}

void AdvancedDebugger::AnalyzeWinMain( PreExecuteEvent &event )
{
    if (event.Cpu->GetCurrentModule() != 0) return;
    if (event.Inst->Main.Inst.Opcode != 0xe8) return;   // call rel32
    u32 rel = event.Inst->Main.Inst.AddrValue;
    u32 eip = event.Cpu->EIP;
    if (rel >= eip) return;      // forward call

    m_callsBackward++;

    static const u32 ExitBegin  = 0x15;
    static const u32 ExitEng    = 0x19;

    for (u32 addr = eip + ExitBegin; addr <= eip + ExitEng; addr++) {
        InstPtr pinst = m_disasm->GetInst(event.Cpu, addr);
        if (pinst == NULL) continue;
        if (strstr(pinst->TargetFuncName, "exit") != NULL) {
            m_disasm->GetInst(event.Cpu, eip)->Desc = "WinMain";
            LxInfo("WinMain() found at %08x\n", eip);
            GetEngine()->BreakOnNextInst("WinMain");
        }
    }
}
