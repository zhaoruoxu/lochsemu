#include "stdafx.h"
#include "protocol/analyzer.h"
#include "instruction.h"
#include "taint/taintengine.h"
#include "engine.h"

class LengthFieldDetector : public ProtocolAnalyzer {
public:
    LengthFieldDetector() : ProtocolAnalyzer(1000, "LengthFieldDetector", PreExecuteHandler)
    {
        m_taint = NULL;
    }

    void    Initialize() override
    {
        m_taint = GetProtocol()->GetEngine()->GetTaintEngine();
    }

    void    OnPreExecute(PreExecuteEvent &event) override
    {
        if (event.Cpu->GetCurrentModule() != 0) return;

        if (IsMemoryArg(event.Inst->Main.Argument1)) {
            Taint1 t = m_taint->GetTaintAddressingReg(event.Inst->Main.Argument1);
            if (t.IsAnyTainted()) {
                LxError("Found: %s at [%08x] %s\n", t.T[0].ToString().c_str(), event.Cpu->EIP, 
                    event.Inst->Main.CompleteInstr);
            }
        }
        if (IsMemoryArg(event.Inst->Main.Argument2)) {
            Taint1 t = m_taint->GetTaintAddressingReg(event.Inst->Main.Argument2);
            if (t.IsAnyTainted()) {
                LxError("Found: %s at [%08x] %s\n", t.T[0].ToString().c_str(), event.Cpu->EIP, 
                    event.Inst->Main.CompleteInstr);
            }
        }
    }

private:
    TaintEngine *   m_taint;
};

LengthFieldDetector __haha__whatever_this_is__;