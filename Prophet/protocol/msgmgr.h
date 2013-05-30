#pragma once
 
#ifndef __PROPHET_PROTOCOL_SESSION_H__
#define __PROPHET_PROTOCOL_SESSION_H__

#include "prophet.h"
#include "event.h"
#include "formatsyn.h"
#include "message.h"
#include "taint/taint.h"
#include "runtrace.h"

class MessageManager : ISerializable {
public:
    MessageManager(Protocol *protocol);
    ~MessageManager();

    void            Initialize();
    void            OnPostExecute(PostExecuteEvent &event);
    void            OnMessageBegin(MessageBeginEvent &event);
    void            OnMessageEnd(MessageEndEvent &event);

    Message *       GetCurrentMessage() { return m_message; }
    const Message * GetCurrentMessage() const { return m_message; }
    FormatSyn &     GetFormatSynthesizer() { return m_format; }
    const FormatSyn &   GetFormatSynthesizer() const { return m_format; }
    RunTrace &      GetRunTrace() { return m_tracer; }
    const RunTrace &GetRunTrace() const { return m_tracer; }
    Protocol *      GetProtocol() { return m_protocol; }
    const Protocol *GetProtocol() const { return m_protocol; }

    void            Serialize(Json::Value &root) const override;
    void            Deserialize(Json::Value &root) override;

//     void            OnSubmitFormat(const Taint &t, FieldFormat f);
//     void            OnSubmitFormat(const Taint1 &t, FieldFormat f)
//     {
//         OnSubmitFormat(t[0], f);
//     }

    void            SubmitLengthField(int first, int last, int target);
    void            SubmitToken(byte t, int first, int last);
    void            SubmitFixedLen(int first, int last);
private:
    FormatSyn       m_format;
    Protocol *      m_protocol;
    Message *       m_message;
    //TaintEngine *   m_taint;
    RunTrace        m_tracer;
    bool            m_tracing;

    bool            m_breakOnMsgBegin;
    bool            m_breakOnMsgEnd;
    bool            m_autoShowMemory;
};

#endif // __PROPHET_PROTOCOL_SESSION_H__