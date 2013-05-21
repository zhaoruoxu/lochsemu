#include "stdafx.h"
#include "msgformat.h"
#include "protocol/taint/taintengine.h"


MessageFieldFormat::MessageFieldFormat( TaintEngine *taint )
{
    m_taint = taint;
    Reset();
}

MessageFieldFormat::~MessageFieldFormat()
{
    Reset();
}

void MessageFieldFormat::Reset()
{

}

void MessageFieldFormat::OnExecuteTrace( ExecuteTraceEvent &event )
{
    const TContext *t = event.Context;
    if (!Instruction::IsCmp(event.Context->Inst)) return;

    u32 token = t->Oper(t->Inst->Main.Argument2);

    if (token > 0xff) return;

    Taint1 taint = m_taint->GetTaintShrink(t, t->Inst->Main.Argument1);
    if (!taint.IsAnyTainted()) return;

    if (t->Flag(InstContext::ZF)) {
        m_tokens[token].HasTrueCompare = true;
    } else {
        m_tokens[token].HasFalseCompare = true;
    }

//     LxInfo("%08x %-50s token=%02x taint=%s\n", t->Eip, t->Inst->Main.CompleteInstr,
//         token, taint[0].ToString().c_str());
}

void MessageFieldFormat::OnComplete()
{
    for (int i = 0; i < 256; i++) {
        if (m_tokens[i].HasTrueCompare || m_tokens[i].HasFalseCompare) {
            LxInfo("possible token: %02x %c\n", i, isprint(i) ? i : '.');
        }
    }
}
