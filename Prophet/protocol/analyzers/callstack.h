#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_CALLSTACK_H__
#define __PROPHET_PROTOCOL_ANALYZERS_CALLSTACK_H__
 
#include "protocol/analyzer.h"
#include "procscope.h"

typedef std::vector<Procedure *> ProcStack;

u32 GetProcStackHash(const ProcStack &stack);

class CallStack : public TraceAnalyzer {
public:
    CallStack(ProcScope *procs);
    virtual ~CallStack();

    virtual void Reset() override;
    virtual void OnExecuteTrace(ExecuteTraceEvent &event) override;

    ProcStack &Get() { return m_stack; }
    const ProcStack &Get() const { return m_stack; }

private:
    ProcScope * m_procs;
    ProcStack   m_stack;
    const TContext *m_prev;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_CALLSTACK_H__