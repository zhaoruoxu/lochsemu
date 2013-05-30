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
    virtual void OnProcBegin(ExecuteTraceEvent &event) override;
    virtual void OnProcEnd(ExecuteTraceEvent &event) override;
    virtual void OnComplete() override;

    ProcStack &Get() { return m_stack; }
    const ProcStack &Get() const { return m_stack; }
    Procedure * Top() { return m_stack.back(); }
    const Procedure * Top() const { return m_stack.back(); }

private:
    ProcScope * m_procs;
    ProcStack   m_stack;
    // const TContext *m_prev;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_CALLSTACK_H__