#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_TRACEEXEC_H__
#define __PROPHET_PROTOCOL_ANALYZERS_TRACEEXEC_H__
 
#include "protocol/analyzer.h"
#include "protocol/runtrace.h"

/*
 * 运行其他TraceAnalyzer的驱动器
 */

class TraceExec : public TraceAnalyzer {
public:
    TraceExec(const RunTrace &t);
    virtual void OnExecuteTrace(ExecuteTraceEvent &event) override;
    virtual void OnComplete() override;
    virtual void Reset() override;
    void Add(TraceAnalyzer *t);
    void Add(TraceAnalyzer *t0, TraceAnalyzer *t1);
    void Add(TraceAnalyzer *t0, TraceAnalyzer *t1, TraceAnalyzer *t2);
    void RunPartial(int firstIncl, int lastIncl);
    void RunMessage(const Message *msg);

private:
    bool IsSpecialCallPop(ExecuteTraceEvent &event) const;
private:
    const RunTrace &m_trace;
    static const int MaxAnalyzers = 16;
    TraceAnalyzer * m_workers[MaxAnalyzers];
    int m_count;
    const TContext *    m_prev;
};

#endif // __PROPHET_PROTOCOL_ANALYZERS_TRACEEXEC_H__