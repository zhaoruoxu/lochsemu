#include "stdafx.h"
#include "direction_field.h"
#include "instruction.h"
#include "engine.h"
#include "taint/taintengine.h"
#include "protocol/msgmgr.h"

DirectionField::DirectionField( Protocol *protocol )
    : ProtocolAnalyzer(protocol, "DirectionField", PreExecuteHandler)
{
    m_taint = NULL;
}

void DirectionField::Initialize()
{
    m_taint = m_protocol->GetEngine()->GetTaintEngine();
    m_msgmgr = m_protocol->GetMessageManager();
}

void DirectionField::OnPreExecute( PreExecuteEvent &event )
{
    if (event.Cpu->GetCurrentModule() != 0) return;

    if (IsMemoryArg(event.Inst->Main.Argument1)) {
        Taint1 t = m_taint->GetTaintAddressingReg(event.Inst->Main.Argument1);
        if (t.IsAnyTainted()) {
            m_msgmgr->OnSubmitFormat(t.T[0], FieldFormat::Length);
        }
    }
    if (IsMemoryArg(event.Inst->Main.Argument2)) {
        Taint1 t = m_taint->GetTaintAddressingReg(event.Inst->Main.Argument2);
        if (t.IsAnyTainted()) {
            m_msgmgr->OnSubmitFormat(t.T[0], FieldFormat::Length);
        }
    }
}
