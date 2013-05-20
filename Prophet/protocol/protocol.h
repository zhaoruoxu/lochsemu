#pragma once
 
#ifndef __PROPHET__PROTOCOL_H__
#define __PROPHET__PROTOCOL_H__
 
#include "prophet.h"
#include "utilities.h"
#include "apiprocessor.h"
#include "formatsyn.h"
#include "msgmgr.h"

class ExecuteTraceEvent : public Event {
public:
    ExecuteTraceEvent(void *sender) : Event(sender), Context(NULL), Seq(-1) {}
    ExecuteTraceEvent(void *sender, const TContext *ctx, int seq) : Event(sender),
        Context(ctx), Seq(seq) {}

    const TContext *const Context;
    const int   Seq;
};

class Protocol : public ISerializable {
public:
    enum State {
        Idle,
        ProcessingMessage,
    };

public:
    Protocol(ProEngine *engine);
    ~Protocol();

    void        Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool        IsEnabled() const { return m_enabled; }

    void        RegisterAnalyzer(ProtocolAnalyzer *analyzer);

    void        Initialize();
    void        OnPreExecute        (PreExecuteEvent        &event);
    void        OnPostExecute       (PostExecuteEvent       &event);
    void        OnMemRead           (MemReadEvent           &event);
    void        OnMemWrite          (MemWriteEvent          &event);
    void        OnProcessPreRun     (ProcessPreRunEvent     &event);
    void        OnProcessPostRun    (ProcessPostRunEvent    &event);
    void        OnProcessPreLoad    (ProcessPreLoadEvent    &event);
    void        OnProcessPostLoad   (ProcessPostLoadEvent   &event);
    void        OnWinapiPreCall     (WinapiPreCallEvent     &event);
    void        OnWinapiPostCall    (WinapiPostCallEvent    &event);

    void        OnMessageBegin      (MessageBeginEvent      &event);
    void        OnMessageEnd        (MessageEndEvent        &event);

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;
    State       GetState() const { return m_state; }

    ProEngine *         GetEngine() { return m_engine; }
    const ProEngine *   GetEngine() const { return m_engine; }
    MessageManager *    GetMessageManager() { return &m_msgmanager; }
    const MessageManager *  GetMessageManager() const { return &m_msgmanager; }
    ApiProcessor *      GetApiProcessor() { return &m_apiprocessor; }
    const ApiProcessor *    GetApiProcessor() const { return &m_apiprocessor; }

    void        AddMessage(Message *msg);
    int         GetTotalMessages() const { return m_totalMessages; }

    void        UpdateTContext(const Processor *cpu, TContext *ctx) const;

private:
/*    void        ReorderAnalyzers();*/
//     void        BeginTrace();
//     void        EndTrace(int *nCount = NULL);
private:
    ProEngine *         m_engine;
    TaintEngine *       m_taint;

    static const int    MaxAnalyzers = 64;
    int                 m_totalAnalyzers;
    ProtocolAnalyzer *  m_analyzers[MaxAnalyzers];

    static const int    MaxMessages = 4096;
    int                 m_totalMessages;
    Message *           m_messages[MaxMessages];

    ApiProcessor        m_apiprocessor;
    MessageManager      m_msgmanager;
    
    State               m_state;
    bool                m_enabled;

    u32                 m_eipPreExec;
};
 
#endif // __PROPHET__PROTOCOL_H__