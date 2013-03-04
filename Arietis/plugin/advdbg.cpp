#include "stdafx.h"
#include "plugin.h"
#include "engine.h"
#include "event.h"
#include "static/disassembler.h"

class AdvancedDebugger : public Plugin {
public:
    AdvancedDebugger() : Plugin("Advanced Debugger", Func_PreExecute)
    {
        m_breakOnWinMain    = true;
        m_callsBackward     = 0;
    }

    void    Initialize() override
    {
        m_debugger  = GetEngine()->GetDebugger();
        m_disasm    = GetEngine()->GetDisassembler();
    }

    void    Serialize(Json::Value &root) const override
    {
        Plugin::Serialize(root);

        root["break_on_winmain"] = m_breakOnWinMain;
    }

    void    Deserialize(Json::Value &root) override
    {
        Plugin::Deserialize(root);

        m_breakOnWinMain = root.get("break_on_winmain", m_breakOnWinMain).asBool();
    }

    void    OnPreExecute(PreExecuteEvent &event, bool firstTime) override
    {
        if (!firstTime) return;
        AnalyzeWinMain(event); 
    }

    void    AnalyzeWinMain(PreExecuteEvent &event)
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
            InstPtr pinst = m_disasm->GetInst(addr);
            if (pinst == NULL) continue;
            if (strstr(pinst->FuncName, "exit") != NULL) {
                MarkWinMain(eip);
            }
        }
    }

    void    MarkWinMain(u32 addr)
    {
        LxInfo("WinMain() found at %08x\n", addr);
        if (m_debugger->GetBreakpoint(addr) == NULL) {
            m_debugger->AddBreakpoint(addr, "winmain");
        }
    }

private:
    ADebugger *     m_debugger;
    Disassembler *  m_disasm;

    bool    m_breakOnWinMain;

    int     m_callsBackward;
};

AdvancedDebugger a;