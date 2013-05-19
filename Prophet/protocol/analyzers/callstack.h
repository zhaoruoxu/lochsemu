#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_CALLSTACK_H__
#define __PROPHET_PROTOCOL_ANALYZERS_CALLSTACK_H__
 
#include "protocol/analyzer.h"
#include "procscope.h"

class CallStack : public TraceAnalyzer {
public:
    CallStack(ProcScope *procs);
    virtual ~CallStack();

    virtual void Reset() override;
    virtual void OnExecuteTrace(ExecuteTraceEvent &event) override;

private:
    ProcScope * m_procs;
    std::vector<Procedure *> m_stack;
    const TContext *m_prev;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_CALLSTACK_H__