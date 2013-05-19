#include "stdafx.h"
#include "traceexec.h"

TraceExec::TraceExec()
{
    Reset();
}

void TraceExec::OnExecuteTrace( ExecuteTraceEvent &event )
{
    for (int i = 0; i < m_count; i++)
        m_workers[i]->OnExecuteTrace(event);
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
}

void TraceExec::Add( TraceAnalyzer *t )
{
    if (m_count == MaxAnalyzers) {
        LxFatal("Too many analyzers\n");
    }
    m_workers[m_count++] = t;
}


void TraceExec::Run( const RunTrace &t )
{
    for (int i = 0; i < t.Count(); i++) {
        ExecuteTraceEvent e(this, t.Get(i), i);
        OnExecuteTrace(e);
    }
    OnComplete();
}
