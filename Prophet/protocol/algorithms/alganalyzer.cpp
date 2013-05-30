#include "stdafx.h"
#include "alganalyzer.h"


AlgorithmAnalyzer::AlgorithmAnalyzer( TSnapshot &t )
    : m_snapshot(t)
{

}

void AlgorithmAnalyzer::OnProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    if (ctx.EndSeq - ctx.BeginSeq < 100) return;
    TaintEngine taint;
    taint.TaintRule_LoadMemory();
    taint.ApplySnapshot(m_snapshot);

    ProcContext pc = SingleProcExec::Run(event, ctx, &taint);
    pc.Dump(StdOut());
}
