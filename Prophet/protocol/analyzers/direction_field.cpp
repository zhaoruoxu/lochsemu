#include "stdafx.h"
#include "direction_field.h"
#include "instruction.h"
#include "engine.h"
#include "taint/taintengine.h"
#include "protocol/msgmgr.h"

/*
 * 1) 寻址寄存器被Taint；访问的内存被Taint
 * 2) backwards jump; loop stop condition is tainted; 
 *
 */

DirectionField::DirectionField( Protocol *protocol )
    : ProtocolAnalyzer(protocol, "DirectionField", PreExecuteHandler)
{
    m_taint = NULL;
}

void DirectionField::Initialize()
{
    m_taint = m_protocol->GetEngine()->GetTaintEngine();
    m_msgmgr = m_protocol->GetMessageManager();
    m_disasm = m_protocol->GetEngine()->GetDisassembler();
}

void DirectionField::OnPreExecute( PreExecuteEvent &event )
{
    if (event.Cpu->GetCurrentModule() != 0) return;

    const ARGTYPE &arg1 = event.Inst->Main.Argument1;
    const ARGTYPE &arg2 = event.Inst->Main.Argument2;
    // 读内存时：1）寻址寄存器被Taint；2）内存位置被Taint 

    if (IsMemoryArg(arg2)) {
        Taint1 tReg = m_taint->GetTaintAddressingReg(arg2);
        if (tReg.IsAnyTainted()) {
            Taint1 tMem = m_taint->GetTaintShrink(event.Cpu, arg2);
            if (tMem.IsAnyTainted()) {
                LxDebug("memory\n");
                m_msgmgr->OnSubmitFormat(tReg, FieldFormat::Length);
                m_protocol->GetEngine()->BreakOnNextInst("memory");
            }
        }
    }
    InstPtr inst = m_disasm->GetInst(event.Cpu->EIP);
    if (Instruction::IsConditionalJump(inst)) {
        if ( (inst->Target < event.Cpu->EIP && !event.Cpu->IsJumpTaken(inst)) 
            ||
             (inst->Target > event.Cpu->EIP && event.Cpu->IsJumpTaken(inst)) ) {
            Taint1 tFlag = m_taint->GetTestedFlagTaint(inst);
            if (tFlag.IsAnyTainted()) {
                LxDebug("flag\n");
                m_msgmgr->OnSubmitFormat(tFlag, FieldFormat::Length);
                m_protocol->GetEngine()->BreakOnNextInst("flag");
            }
        }
    }
// 
//     if (IsMemoryArg(event.Inst->Main.Argument1)) {
//         Taint1 t = m_taint->GetTaintAddressingReg(event.Inst->Main.Argument1);
//         if (t.IsAnyTainted()) {
//             LxInfo("arg1\n");
//             m_msgmgr->OnSubmitFormat(t[0], FieldFormat::Length);
//             m_protocol->GetEngine()->BreakOnNextInst("arg1");
//         }
//     }
//     if (IsMemoryArg(event.Inst->Main.Argument2)) {
//         Taint1 t = m_taint->GetTaintAddressingReg(event.Inst->Main.Argument2);
//         if (t.IsAnyTainted()) {
//             LxInfo("arg2\n");
//             m_msgmgr->OnSubmitFormat(t[0], FieldFormat::Length);
//             m_protocol->GetEngine()->BreakOnNextInst("arg2");
//         }
//     }
}
