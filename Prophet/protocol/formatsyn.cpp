#include "stdafx.h"
#include "formatsyn.h"
#include "msgmgr.h"

MsgByteInfo::MsgByteInfo()
{
    ZeroMemory(FormatCount, sizeof(FormatCount));
    FormatCount[FieldFormat::Unknown] = 1;
    Target = -1;
}

FormatSyn::FormatSyn( MessageManager *msgmgr )
    : m_msgmgr(msgmgr)
{
    m_msgInfo = NULL;
    m_message = NULL;

    m_weightLenField        = 10;
    m_weightLenFieldNoTarget = 4;
    m_weightLenFieldVar     = 2;
    m_weightSeparator       = 10;
    m_weightSeparatedVar    = 5;
    m_weightFixedLen        = 2;
}

FormatSyn::~FormatSyn()
{

}

void FormatSyn::Initialize()
{

}

void FormatSyn::OnMessageBegin( MessageBeginEvent &event )
{
    m_message = m_msgmgr->GetCurrentMessage();
    m_msgInfo = new MsgByteInfo[m_message->Size()];
}

void FormatSyn::OnMessageEnd( MessageEndEvent &event )
{
    Synthesize();
    m_message = NULL;
    SAFE_DELETE_ARRAY(m_msgInfo);
}

void FormatSyn::Serialize( Json::Value &root ) const 
{
    root["weight_length_field"]             = m_weightLenField;
    root["weight_length_field_no_target"]   = m_weightLenFieldNoTarget;
    root["weight_length_field_var"]         = m_weightLenFieldVar;
    root["weight_separator"]                = m_weightSeparator;
    root["weight_separated_var"]            = m_weightSeparatedVar;
    root["weight_fixed_len"]                = m_weightFixedLen;
}

void FormatSyn::Deserialize( Json::Value &root )
{
    m_weightLenField = root.get("weight_length_field", 
        m_weightLenField).asInt();
    m_weightLenFieldNoTarget = root.get("weight_length_field_no_target",
        m_weightLenFieldNoTarget).asInt();
    m_weightLenFieldVar = root.get("weight_length_field_var", 
        m_weightLenFieldVar).asInt();
    m_weightSeparator = root.get("weight_separator", m_weightSeparator).asInt();
    m_weightSeparatedVar = root.get("weight_separated_var",
        m_weightSeparatedVar).asInt();
    m_weightFixedLen = root.get("weight_fixed_len", m_weightFixedLen).asInt();
}

void FormatSyn::SubmitLengthField( int first, int last, int target )
{
    Assert(m_message);
    Assert(first <= last);

    for (int i = first; i <= last; i++) {
        m_msgInfo[i].FormatCount[FieldFormat::FixedLen] = 0;
        // Clear FixedLen no matter what
    }

    if (target != -1 && target > last) {
        for (int i = first; i <= last; i++) {
            m_msgInfo[i].FormatCount[FieldFormat::Length] += m_weightLenField;
        }
        if (m_msgInfo[first].Target == -1 || m_msgInfo[first].Target > target) {
            m_msgInfo[first].Target = target;
        }
    } else {
        for (int i = first; i <= last; i++) {
            m_msgInfo[i].FormatCount[FieldFormat::Length] += m_weightLenFieldNoTarget;
        }
    }
}

void FormatSyn::SubmitFixedLenField( int first, int last )
{
    for (int i = first; i <= last; i++) {
        if (m_msgInfo[i].FormatCount[FieldFormat::Length] > 0)
            return;     // skip length field
    }
    for (int i = first; i <= last; i++) {
        m_msgInfo[i].FormatCount[FieldFormat::FixedLen] += m_weightFixedLen;
    }
}

void FormatSyn::SubmitToken( byte t, int first, int last )
{
    Assert(m_message);
    for (int i = first; i <= last; i++) {
        if ((*m_message)[i].Data == t) {
            m_msgInfo[i].FormatCount[FieldFormat::Separator] += m_weightSeparator;
        } else {
            m_msgInfo[i].FormatCount[FieldFormat::VariableLen] += m_weightSeparatedVar;
        }
    }
}

void FormatSyn::Synthesize()
{
    for (int i = 0; i < m_message->Size(); i++) {
        int mostFormat = 0;
        int mostVotes = m_msgInfo[i].FormatCount[0];
        int totalVotes = mostVotes;
        for (int f = 1; f < FieldFormat::Total; f++) {
            if (m_msgInfo[i].FormatCount[f] > mostVotes) {
                mostVotes = m_msgInfo[i].FormatCount[f];
                mostFormat = f;
            }
            totalVotes += m_msgInfo[i].FormatCount[f];
        }
        LxError("%3d %10s %d/%d\n", i, FieldFormatName[mostFormat], mostVotes, totalVotes);
        (*m_message)[i].Format = (FieldFormat) mostFormat;
    }

    // calculate var_field by length field
    int ptr = 0;
    while (ptr < m_message->Size()) {
        int target = m_msgInfo[ptr].Target;
        if (target == -1) {
            ptr++;
            continue;
        }
        Assert(target > ptr);
        for (int i = ptr+1; i < target; i++) {
            if ((*m_message)[i].Format == FieldFormat::Unknown ||
                (*m_message)[i].Format == FieldFormat::FixedLen)
                (*m_message)[i].Format = FieldFormat::VariableLen;
        }
        ptr = target;
    }

    for (int i = 0; i < m_message->Size(); i++) {
        LxError("%3d %02x %10s\n", i, (*m_message)[i].Data, FieldFormatName[(*m_message)[i].Format]);
    }
    
}

