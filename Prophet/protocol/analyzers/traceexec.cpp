#include "stdafx.h"
#include "traceexec.h"
#include "processor.h"
#include "protocol/message.h"

TraceExec::TraceExec(const RunTrace &t)
    : m_trace(t)
{
    Reset();
}

bool TraceExec::IsSpecialCallPop( ExecuteTraceEvent &event ) const
{
    if (m_prev == NULL) return false;
    if (!Instruction::IsCall(m_prev->Inst)) return false;
    if (!Instruction::IsPop(event.Context->Inst)) return false;
    return true;
}

bool TraceExec::IsSpecialXchgJmp( ExecuteTraceEvent &event ) const
{
    if (m_prev == NULL) return false;
    if (!Instruction::IsXchg(m_prev->Inst)) return false;
    if (!Instruction::IsJmp(event.Context->Inst)) return false;
    return true;
}

void TraceExec::OnExecuteTrace( ExecuteTraceEvent &event )
{


    if (event.Seq == 0 || (m_prev && Instruction::IsCall(m_prev->Inst) && 
        !m_prev->HasExecFlag(LX_EXEC_WINAPI_CALL) && 
        !m_prev->HasExecFlag(LX_EXEC_WINAPI_JMP)) && !IsSpecialCallPop(event))
    {
        for (int i = 0; i < m_count; i++)
            m_workers[i]->OnProcBegin(event);
    }
    
    for (int i = 0; i < m_count; i++)
        m_workers[i]->OnExecuteTrace(event);

    if (Instruction::IsRet(event.Context->Inst) || IsSpecialXchgJmp(event)) {
        for (int i = m_count - 1; i >= 0; i--)
            m_workers[i]->OnProcEnd(event);
    }

    m_prev = event.Context;
}

void TraceExec::OnComplete()
{
    for (int i = 0; i < m_count; i++)
        m_workers[i]->OnComplete();
    Reset();
}

void TraceExec::Reset()
{
    m_count = 0;
    ZeroMemory(m_workers, sizeof(m_workers));
    m_prev = NULL;
}

void TraceExec::Add( TraceAnalyzer *t )
{
    if (m_count == MaxAnalyzers) {
        LxFatal("Too many analyzers\n");
    }
    m_workers[m_count++] = t;
}

void TraceExec::Add( TraceAnalyzer *t0, TraceAnalyzer *t1 )
{
    Add(t0); Add(t1);
}

void TraceExec::Add( TraceAnalyzer *t0, TraceAnalyzer *t1, TraceAnalyzer *t2 )
{
    Add(t0); Add(t1); Add(t2);
}

void TraceExec::RunPartial( int firstIncl, int lastIncl )
{
    Assert(firstIncl >= 0 && firstIncl < m_trace.Count());
    Assert(lastIncl >= firstIncl && lastIncl < m_trace.Count());
    for (int i = firstIncl; i <= lastIncl; i++) {
        ExecuteTraceEvent e(this, m_trace.Get(i), i, m_trace);
        OnExecuteTrace(e);
    }
    OnComplete();
}

void TraceExec::RunMessage( const Message *msg )
{
    RunPartial(msg->GetTraceBegin(), msg->GetTraceEnd());
}
