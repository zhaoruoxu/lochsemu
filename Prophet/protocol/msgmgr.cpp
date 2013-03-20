#include "stdafx.h"
#include "msgmgr.h"
#include "engine.h"
#include "message.h"
#include "gui/mainframe.h"

MessageManager::MessageManager( Protocol *protocol )
    : m_protocol(protocol), m_format(this)
{
    m_message = NULL;
}

MessageManager::~MessageManager()
{

}

void MessageManager::Initialize()
{
    m_taint = m_protocol->GetEngine()->GetTaintEngine();
    m_format.Initialize();
}

void MessageManager::OnMessageBegin( MessageBeginEvent &event )
{
    LxError("Message begins: len=%08x, addr=%08x, data=%s\n", 
        event.MessageLen, event.MessageAddr, event.MessageData);
    // set taint
    if (event.MessageLen >= Taint::GetWidth()) {
        LxWarning("Prophet: message length(%d) >= taint width(%d)!\n", event.MessageLen,
            Taint::GetWidth());
    }
    m_taint->TaintMemoryRanged(event.MessageAddr, event.MessageLen, false);
    m_message = new Message(event.MessageLen, event.MessageData);
    // TODO : other stuff
    m_format.OnMessageBegin(event);
}

void MessageManager::OnMessageEnd( MessageEndEvent &event )
{
    LxError("Message ends: len=%08x, addr=%08x, data=%s\n",
        event.MessageLen, event.MessageAddr, event.MessageData);

    m_format.OnMessageEnd(event);

    // clear taint
    Taint1 t = m_taint->MemTaint.Get<1>(event.MessageAddr);
    if (t.IsAnyTainted()) {
        LxError("has tainted buffer byte\n");
    }
    m_protocol->AddMessage(m_message);
    m_protocol->GetEngine()->GetGUI()->ShowMessage(m_message);
    m_protocol->GetEngine()->BreakOnNextInst("Message end");
    m_message = NULL;
}

void MessageManager::Serialize( Json::Value &root ) const 
{
    // TODO : msgmgr stuff
    Json::Value formatsyn;
    m_format.Serialize(formatsyn);
    root["format_synthesizer"] = formatsyn;
}

void MessageManager::Deserialize( Json::Value &root )
{
    // TODO : msgmgr stuff
    Json::Value formatsyn = root["format_synthesizer"];
    if (!formatsyn.isNull())
        m_format.Deserialize(formatsyn);
}

void MessageManager::OnSubmitFormat( const Taint &t, FieldFormat f )
{
    LxWarning("Format submitted: %d %s\n", f, t.ToString().c_str());
}

