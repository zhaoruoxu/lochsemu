#include "stdafx.h"
#include "msgmgr.h"
#include "engine.h"
#include "message.h"


MessageManager::MessageManager( Protocol *protocol )
    : m_protocol(protocol)
{
    m_message = NULL;
}

MessageManager::~MessageManager()
{

}

void MessageManager::Initialize()
{
    m_taint = m_protocol->GetEngine()->GetTaintEngine();
}

void MessageManager::OnMessageBegin( MessageBeginEvent &event )
{
    LxError("Session begins: len=%08x, addr=%08x, data=%s\n", 
        event.MessageLen, event.MessageAddr, event.MessageData);
    // set taint
    m_taint->TaintMemoryRanged(event.MessageAddr, event.MessageLen, false);
    m_message = new Message(event.MessageLen);
    // TODO : other stuff
}

void MessageManager::OnMessageEnd( MessageEndEvent &event )
{
    LxError("Session ends: len=%08x, addr=%08x, data=%s\n",
        event.MessageLen, event.MessageAddr, event.MessageData);
    // clear taint
    Taint1 t = m_taint->MemTaint.Get<1>(event.MessageAddr);
    if (t.IsAnyTainted()) {
        LxError("has tainted buffer byte\n");
    }
    m_protocol->AddMessage(m_message);
    m_message = NULL;
}
