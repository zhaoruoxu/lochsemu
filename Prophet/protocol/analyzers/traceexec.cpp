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
    for (int i = 0; i < t.Count(); i++) {
        ExecuteTraceEvent e(this, t.Get(i), i, t.Count());
        OnExecuteTrace(e);
    }
    OnComplete();
}
