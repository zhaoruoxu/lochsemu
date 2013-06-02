#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_ALGANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_ALGANALYZER_H__
 
#include "protocol/analyzers/procexec.h"
#include "protocol/taint/taintengine.h"

class AlgorithmAnalyzer;

class AdvAlgEngine : public ProcAnalyzer {
public:
    AdvAlgEngine(MessageManager *msgmgr, Message *msg, int minProcSize = 32);
    virtual ~AdvAlgEngine();
    virtual void OnProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual void OnComplete() override;

    TaintEngine *   GetTaint() { return &m_taint; }
    const TaintEngine *GetTaint() const { return &m_taint; }
    Message * GetMessage() { return m_message; }
    MessageManager *    GetMessageManager() { return m_msgmgr; }


private:
    void RegisterAnalyzers();
    void RegisterAnalyzer(AlgorithmAnalyzer *a);
private:
    MessageManager *m_msgmgr;
    Message *m_message;
    TaintEngine m_taint;
    int m_minProcSize;

    static const int MaxAnalyzers = 16;
    AlgorithmAnalyzer * m_analyzers[MaxAnalyzers];
    int m_count;
};

class AlgorithmAnalyzer {
public:
    AlgorithmAnalyzer();
    virtual ~AlgorithmAnalyzer() {}
    virtual void OnProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) {}
    virtual void OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) {}
    virtual void OnInputProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) {}
    virtual void OnComplete() {}

    void SetAlgEngine(AdvAlgEngine *e) { m_algEngine = e; }
protected:
    AdvAlgEngine *  m_algEngine;
};
 
#endif // __PROPHET_PROTOCOL_ALGORITHMS_ALGANALYZER_H__