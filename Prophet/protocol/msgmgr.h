#pragma once
 
#ifndef __PROPHET_PROTOCOL_SESSION_H__
#define __PROPHET_PROTOCOL_SESSION_H__

#include "prophet.h"
#include "event.h"
#include "formatsyn.h"

class MessageManager : ISerializable {
public:
    MessageManager(Protocol *protocol);
    ~MessageManager();

    void            Initialize();
    void            OnMessageBegin(MessageBeginEvent &event);
    void            OnMessageEnd(MessageEndEvent &event);

    Message *       GetCurrentMessage() { return m_message; }
    const Message * GetCurrentMessage() const { return m_message; }
    FormatSyn *     GetFormatSynthesizer() { return &m_format; }
    const FormatSyn *   GetFormatSynthesizer() const { return &m_format; }

    void            Serialize(Json::Value &root) const override;
    void            Deserialize(Json::Value &root) override;
private:
    FormatSyn       m_format;
    Protocol *      m_protocol;
    Message *       m_message;
    TaintEngine *   m_taint;
};

#endif // __PROPHET_PROTOCOL_SESSION_H__