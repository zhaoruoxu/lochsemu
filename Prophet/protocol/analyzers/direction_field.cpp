#include "stdafx.h"
#include "direction_field.h"
#include "protocol/msgmgr.h"
#include "msgtree.h"
#include "protocol/taint/taintengine.h"

/*
 * 1) 寻址寄存器被Taint；访问的内存被Taint
 * 2) backwards jump; loop stop condition is tainted; 
 *
 */

// 
// DirectionField::DirectionField( Protocol *protocol )
//     : ProtocolAnalyzer(protocol, "DirectionField")
// {
//     //m_taint = NULL;
//     m_useFlag = true;
// }
// 
// void DirectionField::Initialize()
// {
//     //m_taint = m_protocol->GetEngine()->GetTaintEngine();
//     m_msgmgr = m_protocol->GetMessageManager();
//     m_disasm = m_protocol->GetEngine()->GetDisassembler();
// }
// 
// void DirectionField::OnPreExecute( PreExecuteEvent &event )
// {
//     
// //     if (event.Cpu->GetCurrentModule() != 0) return;
// // 
// //     if (IsMemoryArg(event.Inst->Main.Argument2) && event.Inst->Main.Inst.Opcode != 0x8d /* LEA */) {
// //         if (!CheckMemoryLength(event, event.Inst->Main.Argument2)) {
// //             // check fixed length
// //             Taint1 t = m_taint->GetTaintShrink(event.Cpu, event.Inst->Main.Argument2);
// //             if (t.IsAnyTainted()) {
// //                 int first, last;
// //                 GetTaintRange(t[0], &first, &last);
// //                 m_msgmgr->SubmitFixedLen(first, last);
// //                 //m_protocol->GetEngine()->BreakOnNextInst("fixed len");
// //             }
// //         }
// //     }
// //     if (m_useFlag) {
// //         CheckFlag(event);
// //     }
// }
// 
// void DirectionField::Serialize( Json::Value &root ) const 
// {
//     root["use_flag"] = m_useFlag;
// }
// 
// void DirectionField::Deserialize( Json::Value &root )
// {
//     m_useFlag = root.get("use_flag", m_useFlag).asBool();
// }
// 
// bool DirectionField::CheckMemoryLength( PreExecuteEvent &event, const ARGTYPE &arg )
// {
// //     Taint1 tReg = m_taint->GetTaintAddressingReg(event.Cpu, arg);
// //     if (!tReg.IsAnyTainted()) return false;
// // 
// //     Taint1 tMem = m_taint->GetTaintShrink(event.Cpu, arg);
// //     if (!tMem.IsAnyTainted()) return false;
// // 
// //     int first, last, target;
// //     GetTaintRange(tReg[0], &first, &last);
// //     GetTaintRange(tMem[0], &target, NULL);
// //     m_msgmgr->SubmitLengthField(first, last, target);
//     return true;
// }
// 
// void DirectionField::CheckFlag( PreExecuteEvent &event )
// {
//     InstPtr inst = m_disasm->GetInst(event.Cpu->EIP);
//     if (!Instruction::IsConditionalJump(inst)) return;
// 
//     /*
//      * target > eip: forwards jump, taken:end of loop
//      * target < eip: backwards jump, not taken:end of loop
//      */
// //     u32 eip = event.Cpu->EIP;
// //     if (m_inloop.find(eip) == m_inloop.end()) {
// //         bool inLoop = (inst->Target > eip && !event.Cpu->IsJumpTaken(inst))
// //             || (inst->Target < eip && event.Cpu->IsJumpTaken(inst));
// //         if (inLoop) m_inloop.insert(eip);
// //         return;
// //     }
// // 
// //     bool endLoop = (inst->Target > eip && event.Cpu->IsJumpTaken(inst))
// //         || (inst->Target < eip && !event.Cpu->IsJumpTaken(inst));
// // 
// //     if (!endLoop) return;
// // 
// //     m_inloop.erase(eip);
// // 
// //     Taint1 tFlag = m_taint->GetTestedFlagTaint(event.Cpu, inst);
// //     if (!tFlag.IsAnyTainted()) return;
// // 
// //     int first, last;
// //     GetTaintRange(tFlag[0], &first, &last);
// //     m_msgmgr->SubmitLengthField(first, last, -1);
//     //m_protocol->GetEngine()->BreakOnNextInst("flag");
// }
// 
// void DirectionField::OnMessageBegin( MessageBeginEvent &event )
// {
// 
// }
// 
// void DirectionField::OnMessageEnd( MessageEndEvent &event )
// {
//     m_inloop.clear();
// }

DirectionField::DirectionField( Message *msg, TaintEngine *taint )
{
    m_message = msg;
    m_taint = taint;
}

DirectionField::~DirectionField()
{

}

void DirectionField::Reset()
{

}

void DirectionField::OnExecuteTrace( ExecuteTraceEvent &event )
{
    const TContext *ctx = event.Context;
    pbyte p = (pbyte) &ctx->Mr.Val;
    for (u32 i = 0; i < ctx->Mr.Len; i++) {
        u32 addr = ctx->Mr.Addr + i;
        if (m_message->GetRegion().Contains(addr)) {
            m_currentNode = m_message->GetTree()->FindNode(addr);
        }
    }

    if (IsMemoryArg(ctx->Inst->Main.Argument1) && ctx->Inst->Main.Inst.Opcode != 0x8d) {
        Taint t = m_taint->GetTaintAddressingReg(ctx, ctx->Inst->Main.Argument1)[0];
        if (!t.IsAnyTainted()) return;
        auto trs = t.GenerateRegions();
        for (auto &tr : trs) {
            if (tr.Len > 4 /*|| tr.Len == 1*/) continue;
            if (tr.Len + tr.Offset > m_currentNode->L()) continue;
            LxInfo("[%d-%d] -> [%d-%d]\n", tr.Offset, tr.Offset + tr.Len - 1,
                m_currentNode->L(), m_currentNode->R());
            Analyze(tr, m_currentNode);
        }
    }

//     if (m_currentNode && Instruction::IsCmpOrTest(ctx->Inst)) {
//         Taint t1 = m_taint->GetTaintShrink(ctx, ctx->Inst->Main.Argument1);
//         if (!t1.IsAnyTainted()) return;
//         auto trs = t1.GenerateRegions();
//         for (auto &tr : trs) {
//             if (tr.Len > 4) continue;
//             LxInfo("[%d-%d] -> [%d-%d]\n", tr.Offset, tr.Offset + tr.Len - 1,
//                 m_currentNode->L(), m_currentNode->R());
//         }
//     }
}

void DirectionField::OnComplete()
{

}

void DirectionField::Analyze( const TaintRegion &tr, MessageTreeNode *node )
{
    MessageTreeNode *from = m_message->GetTree()->FindOrCreateNode(tr);
    if (from == NULL) return;

    auto iter = m_discovered.find(from);
    if (iter != m_discovered.end() && iter->second == node) return;

    if (tr.Len == 4) {
        int val = ((int *) (m_message->GetRaw() + tr.Offset))[0];
        if (val >= 0 && val < m_message->Size()) {
            while (node->GetParent() && node->Length() < val) {
                node = node->GetParent();
            }
        }
    }

    auto iter1 = m_discovered.find(from);
    if (iter1 != m_discovered.end() && iter1->second == node) return;

    from->AddLink(NodeLink(node, m_message, "Len", "forward"));
    LxInfo("Length linked added: %d-%d -> %d-%d\n", tr.Offset, tr.Offset + tr.Len - 1,
        node->L(), node->R());
    m_discovered[from] = node;
}
