#include "stdafx.h"
#include "traceexec.h"
#include "processor.h"

TraceExec::TraceExec()
{
    Reset();
}

void TraceExec::OnExecuteTrace( ExecuteTraceEvent &event )
{
    if (event.Seq == 0 || (m_prev && Instruction::IsCall(m_prev->Inst) && 
        !m_prev->HasExecFlag(LX_EXEC_WINAPI_CALL) && 
        !m_prev->HasExecFlag(LX_EXEC_WINAPI_JMP)) )
    {
        for (int i = 0; i < m_count; i++)
            m_workers[i]->OnProcBegin(event);
    }
    
    for (int i = 0; i < m_count; i++)
        m_workers[i]->OnExecuteTrace(event);

    if (Instruction::IsRet(event.Context->Inst)) {
        for (int i = m_count - 1; i >= 0; i--)
            m_workers[i]->OnProcEnd(event);
    }

    m_prev = event.Context;
}

void TraceExec::OnComplete()
{
    for (int i = 0; i < m_count; i++)
        m_workers[i]->OnComplete();
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

void TraceExec::Run( const RunTrace &t )
{
    RunPartial(t, 0, t.Count()-1);
}

void TraceExec::RunPartial( const RunTrace &t, int firstIncl, int lastIncl )
{
    Assert(firstIncl >= 0 && firstIncl < t.Count());
    Assert(lastIncl >= firstIncl && lastIncl < t.Count());
    for (int i = firstIncl; i <= lastIncl; i++) {
        ExecuteTraceEvent e(this, t.Get(i), i, t);
        OnExecuteTrace(e);
    }
    OnComplete();
}
