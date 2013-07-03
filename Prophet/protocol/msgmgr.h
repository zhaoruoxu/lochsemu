#pragma once
 
#ifndef __PROPHET_PROTOCOL_SESSION_H__
#define __PROPHET_PROTOCOL_SESSION_H__

#include "prophet.h"
#include "event.h"
#include "taint/taint.h"
#include "runtrace.h"
#include "dbtrace.h"

class MessageManager : ISerializable {
public:
    MessageManager(Protocol *protocol);
    ~MessageManager();

    void            Initialize();
	void			OnProcessPreRun(ProcessPreRunEvent &event);
    void            OnPostExecute(PostExecuteEvent &event);
    void            OnMessageBegin(MessageBeginEvent &event);
    void            OnMessageEnd(MessageEndEvent &event);

    void            EnqueueMessage(Message *msg, int beginIncl, int endIncl);
    void            Analyze();
    void            GenerateOutput();

	void			TraceMessage(Message *msg, int beginIncl, int endIncl);

//     Message *       GetCurrentMessage() { return m_currRootMsg; }
//     const Message * GetCurrentMessage() const { return m_currRootMsg; }
    RunTrace &      GetRunTrace() { return m_tracer; }
    const RunTrace &GetRunTrace() const { return m_tracer; }
    Protocol *      GetProtocol() { return m_protocol; }
    const Protocol *GetProtocol() const { return m_protocol; }
    TaintEngine *   GetTaint() { return m_taint; }
    const TaintEngine *GetTaint() const { return m_taint; }

    void            Serialize(Json::Value &root) const override;
    void            Deserialize(Json::Value &root) override;

private:
    TaintEngine *   m_taint;
    Protocol *      m_protocol;
    Message *       m_currRootMsg;
    RunTrace        m_tracer;
	DBTrace			m_dbtracer;
    bool            m_tracing;
    bool            m_clearSubNodes;

    int             m_currId;
    std::vector<Message *>  m_messages;
    std::deque<Message *>   m_msgQueue;

    bool            m_breakOnMsgBegin;
    bool            m_breakOnMsgEnd;
    bool            m_autoShowMemory;
};

#endif // __PROPHET_PROTOCOL_SESSION_H__