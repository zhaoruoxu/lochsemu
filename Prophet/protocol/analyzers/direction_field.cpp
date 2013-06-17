#include "stdafx.h"
#include "direction_field.h"
#include "protocol/msgmgr.h"
#include "msgtree.h"
#include "protocol/taint/taintengine.h"


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
            Analyze(tr, m_currentNode);
        }
    }
}

void DirectionField::OnComplete()
{

}

void DirectionField::Analyze( const TaintRegion &tr, TreeNode *node )
{
    TreeNode *from = m_message->GetTree()->FindOrCreateNode(tr);
    if (from == NULL) return;

    auto iter = m_discovered.find(from);
    if (iter != m_discovered.end() && iter->second == node) return;

    while (!from->IsLeaf() && from->Length() > tr.Len) {
        from = from->GetChild(0);
    }

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
    LxInfo("Length linked added: %d:%d -> %d:%d\n", tr.Offset, tr.Len,
        node->L(), node->R() - node->L() + 1);
    m_discovered[from] = node;
}
