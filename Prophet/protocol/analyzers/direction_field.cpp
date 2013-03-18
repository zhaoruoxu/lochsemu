#include "stdafx.h"
#include "direction_field.h"
#include "instruction.h"
#include "engine.h"
#include "taint/taintengine.h"

DirectionField::DirectionField( Protocol *protocol )
    : ProtocolAnalyzer(protocol, "DirectionField", PreExecuteHandler)
{
    m_taint = NULL;
}

void DirectionField::Initialize()
{
    m_taint = GetProtocol()->GetEngine()->GetTaintEngine();
}

void DirectionField::OnPreExecute( PreExecuteEvent &event )
{
    if (event.Cpu->GetCurrentModule() != 0) return;

//     if (IsMemoryArg(event.Inst->Main.Argument1)) {
//         Taint1 t = m_taint->GetTaintAddressingReg(event.Inst->Main.Argument1);
//         if (t.IsAnyTainted()) {
//             LxError("Found: %s at [%08x] %s\n", t.T[0].ToString().c_str(), event.Cpu->EIP, 
//                 event.Inst->Main.CompleteInstr);
//         }
//     }
//     if (IsMemoryArg(event.Inst->Main.Argument2)) {
//         Taint1 t = m_taint->GetTaintAddressingReg(event.Inst->Main.Argument2);
//         if (t.IsAnyTainted()) {
//             LxError("Found: %s at [%08x] %s\n", t.T[0].ToString().c_str(), event.Cpu->EIP, 
//                 event.Inst->Main.CompleteInstr);
//         }
//     }
}
