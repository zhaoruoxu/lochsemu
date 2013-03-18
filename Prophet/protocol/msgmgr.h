#pragma once
 
#ifndef __PROPHET_PROTOCOL_SESSION_H__
#define __PROPHET_PROTOCOL_SESSION_H__

#include "prophet.h"
#include "event.h"

class MessageManager {
public:
    MessageManager(Protocol *protocol);
    ~MessageManager();


    void            Initialize();
    void            OnMessageBegin(MessageBeginEvent &event);
    void            OnMessageEnd(MessageEndEvent &event);
private:
    Protocol *      m_protocol;
    Message *       m_message;
};

#endif // __PROPHET_PROTOCOL_SESSION_H__