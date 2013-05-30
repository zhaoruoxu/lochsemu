#include "stdafx.h"
#include "separator.h"
#include "instruction.h"
#include "engine.h"

SeparatorKeyword::SeparatorKeyword( Protocol *protocol )
    : ProtocolAnalyzer(protocol, "SeparatorKeyword")
{
    m_msglen = 0;
    m_tokenMinCmpRange = 5;
}

void SeparatorKeyword::Initialize()
{
    //m_taint = m_protocol->GetEngine()->GetTaintEngine();
    m_msgmgr = m_protocol->GetMessageManager();
    Reset();
}

static bool IsComparison(const Instruction *inst)
{
    const char *m = inst->Main.Inst.Mnemonic;
    return m[0] == 'c' && m[1] == 'm' && m[2] == 'p';
}

void SeparatorKeyword::OnPreExecute( PreExecuteEvent &event )
{

}

void SeparatorKeyword::OnPostExecute( PostExecuteEvent &event )
{
    if (event.Cpu->GetCurrentModule() != 0) return;
    if (!IsComparison(event.Inst)) return;
    if (!IsConstantArg(event.Inst->Main.Argument2)) return;
    if (event.Inst->Main.Argument2.ArgSize != 8) return;

    byte token = static_cast<byte>(event.Inst->Main.Inst.Immediat);

    // only cmp XXX, YY is supported, where YY is a constant byte
    /// Taint1 t = m_taint->GetTaintShrink(event.Cpu, event.Inst->Main.Argument1);
    /// if (!t.IsAnyTainted()) return;



    /// m_tokenTable[token] |= t[0];

    if (event.Cpu->ZF != 0) {
        m_hasCmpSuccessed[token]    = true;
    } else {
        m_hasCmpFailed[token]       = true;
    }
}

void SeparatorKeyword::OnMessageBegin( MessageBeginEvent &event )
{
    m_msglen = event.MessageLen;
}

void SeparatorKeyword::OnMessageEnd( MessageEndEvent &event )
{
    for (int i = 0; i < 256; i++) {
        if (!m_hasCmpSuccessed[i] || !m_hasCmpFailed[i]) continue;
        CheckToken(i);
    }

    Reset();
}

void SeparatorKeyword::Reset()
{
    m_msglen = 0;
    for (int i = 0; i < 256; i++) {
        m_tokenTable[i].ResetAll();
        m_hasCmpSuccessed[i] = m_hasCmpFailed[i] = false;
    }
}

void SeparatorKeyword::CheckToken( byte t )
{
    int first, last;
    GetTaintRange(m_tokenTable[t], &first, &last);
    if (last - first < m_tokenMinCmpRange) return;
    if (!m_tokenTable[t].IsRangeAllTainted(first, last)) return;

    m_msgmgr->SubmitToken(t, first, last);
}

void SeparatorKeyword::Serialize( Json::Value &root ) const 
{
    root["token_minimum_cmp_range"] = m_tokenMinCmpRange;
}

void SeparatorKeyword::Deserialize( Json::Value &root )
{
    m_tokenMinCmpRange = root.get("token_minimum_cmp_range", 
        m_tokenMinCmpRange).asInt();
}
