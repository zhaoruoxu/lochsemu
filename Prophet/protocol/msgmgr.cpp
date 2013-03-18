#include "stdafx.h"
#include "msgmgr.h"


MessageManager::MessageManager( Protocol *protocol )
    : m_protocol(protocol)
{

}

MessageManager::~MessageManager()
{

}

void MessageManager::Initialize()
{

}

void MessageManager::OnMessageBegin( MessageBeginEvent &event )
{
    LxError("Session begins: len=%08x, addr=%08x, data=%s\n", 
        event.MessageLen, event.MessageAddr, event.MessageData);
}

void MessageManager::OnSessionEnd( MessageEndEvent &event )
{
    LxError("Session ends: len=%08x, addr=%08x, data=%s\n",
        event.MessageLen, event.MessageAddr, event.MessageData);
}
