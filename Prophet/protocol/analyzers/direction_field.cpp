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
    : ProtocolAnalyzer(protocol, "DirectionField", 
    PreExecuteHandler | MessageBeginHandler | MessageEndHandler)
{
    m_taint = NULL;
    m_useFlag = true;
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

    if (IsMemoryArg(event.Inst->Main.Argument2)) {
        CheckMemory(event, event.Inst->Main.Argument2);
    }
    if (m_useFlag) {
        CheckFlag(event);
    }
}

void DirectionField::Serialize( Json::Value &root ) const 
{
    root["use_flag"] = m_useFlag;
}

void DirectionField::Deserialize( Json::Value &root )
{
    m_useFlag = root.get("use_flag", m_useFlag).asBool();
}

void DirectionField::CheckMemory( PreExecuteEvent &event, const ARGTYPE &arg )
{
    Taint1 tReg = m_taint->GetTaintAddressingReg(arg);
    if (!tReg.IsAnyTainted()) return;

    Taint1 tMem = m_taint->GetTaintShrink(event.Cpu, arg);
    if (!tMem.IsAnyTainted()) return;

    LxDebug("memory\n");
    int first, last, target;
    GetTaintRange(tReg[0], &first, &last);
    GetTaintRange(tMem[0], &target, NULL);
    m_msgmgr->SubmitLengthField(first, last, target);
    //m_protocol->GetEngine()->BreakOnNextInst("memory");
}

void DirectionField::CheckFlag( PreExecuteEvent &event )
{
    InstPtr inst = m_disasm->GetInst(event.Cpu->EIP);
    if (!Instruction::IsConditionalJump(inst)) return;

    /*
     * target > eip: forwards jump, taken:end of loop
     * target < eip: backwards jump, not taken:end of loop
     */
    u32 eip = event.Cpu->EIP;
    if (m_inloop.find(eip) == m_inloop.end()) {
        bool inLoop = (inst->Target > eip && !event.Cpu->IsJumpTaken(inst))
            || (inst->Target < eip && event.Cpu->IsJumpTaken(inst));
        if (inLoop) m_inloop.insert(eip);
        return;
    }

    bool endLoop = (inst->Target > eip && event.Cpu->IsJumpTaken(inst))
        || (inst->Target < eip && !event.Cpu->IsJumpTaken(inst));

    if (!endLoop) return;

    m_inloop.erase(eip);

    Taint1 tFlag = m_taint->GetTestedFlagTaint(inst);
    if (!tFlag.IsAnyTainted()) return;

    LxDebug("flag\n");
    int first, last;
    GetTaintRange(tFlag[0], &first, &last);
    m_msgmgr->SubmitLengthField(first, last, -1);
    //m_protocol->GetEngine()->BreakOnNextInst("flag");
}

void DirectionField::OnMessageBegin( MessageBeginEvent &event )
{

}

void DirectionField::OnMessageEnd( MessageEndEvent &event )
{
    m_inloop.clear();
}
