#pragma once
 
#ifndef __PROPHET__PROTOCOL_H__
#define __PROPHET__PROTOCOL_H__
 
#include "Prophet.h"
#include "utilities.h"
#include "analyzer.h"
#include "controller.h"

class Protocol : public ISerializable {

    enum State {
        InSession,
        BetweenSession,
    };

public:
    Protocol(ProEngine *engine);
    ~Protocol();

    void        Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool        IsEnabled() const { return m_enabled; }

    void        SetInSession() { m_state = InSession; }
    void        SetBetweenSession() { m_state = BetweenSession; }
    
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

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

    ProEngine *         GetEngine() { return m_engine; }
    const ProEngine *   GetEngine() const { return m_engine; }

private:
    void        ReorderAnalyzers();
private:
    ProEngine * m_engine;

    static const int    MaxAnalyzers = 64;
    int         m_totalAnalyzers;
    ProtocolAnalyzer *  m_analyzers[MaxAnalyzers];

    Controller  m_controller;
    State       m_state;
    bool        m_enabled;
};
 
#endif // __PROPHET__PROTOCOL_H__