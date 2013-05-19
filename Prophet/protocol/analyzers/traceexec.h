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
    TraceExec();
    virtual void OnExecuteTrace(ExecuteTraceEvent &event) override;
    virtual void OnComplete() override;
    virtual void Reset() override;
    void Add(TraceAnalyzer *t);
    void Run(const RunTrace &t);

private:
    static const int MaxAnalyzers = 16;
    TraceAnalyzer * m_workers[MaxAnalyzers];
    int m_count;
};

#endif // __PROPHET_PROTOCOL_ANALYZERS_TRACEEXEC_H__