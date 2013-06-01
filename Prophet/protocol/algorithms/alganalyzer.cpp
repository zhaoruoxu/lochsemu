#include "stdafx.h"
#include "alganalyzer.h"

#include "rc4_analyzer.h"

AdvAlgEngine::AdvAlgEngine( MessageManager *msgmgr, Message *msg, int minProcSize )
    : m_msgmgr(msgmgr), m_message(msg)
{
    m_minProcSize = minProcSize;
    m_taint.TaintRule_LoadMemory();
    m_count = 0;
    ZeroMemory(m_analyzers, sizeof(m_analyzers));

    RegisterAnalyzers();
}

void AdvAlgEngine::OnProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    if (ctx.EndSeq - ctx.BeginSeq < m_minProcSize) return;

    // TODO : ignore 1, 2 or 3 if not registered

    // 1.
    for (int i = 0; i < m_count; i++)
        m_analyzers[i]->OnProcedure(event, ctx);

    // 2.
    m_taint.Reset();
    m_taint.TaintMemRegion(m_message->GetRegion());
    ProcContext pc = SingleProcExec::Run(event, ctx, &m_taint, 1);
    for (int i = 0; i < m_count; i++)
        m_analyzers[i]->OnOriginalProcedure(event, pc);

    // 3.
    m_taint.Reset();
    for (auto &input : ctx.Inputs) {
        if (!input.second.Tnt.IsAnyTainted())
            m_taint.TaintByte(input.first);
    }
    pc = SingleProcExec::Run(event, ctx, &m_taint, 1);
    for (int i = 0; i < m_count; i++)
        m_analyzers[i]->OnInputProcedure(event, pc);
}

void AdvAlgEngine::RegisterAnalyzer( AlgorithmAnalyzer *a )
{
    if (m_count == MaxAnalyzers) {
        LxFatal("Too many algorithm analyzers\n");
    }
    m_analyzers[m_count++] = a;
    a->SetAlgEngine(this);
}

void AdvAlgEngine::RegisterAnalyzers()
{
    RegisterAnalyzer(new RC4Analyzer());
}

AdvAlgEngine::~AdvAlgEngine()
{
    for (int i = 0; i < m_count; i++)
        SAFE_DELETE(m_analyzers[i]);
}

/*
AlgorithmAnalyzer::AlgorithmAnalyzer( TSnapshot &t )
    : m_snapshot(t)
{
    m_taint.TaintRule_LoadMemory();
}

void AlgorithmAnalyzer::OnProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    if (ctx.EndSeq - ctx.BeginSeq < 100) return;
    //m_taint.TaintRule_LoadMemory();
    m_taint.Reset();
    m_taint.ApplySnapshot(m_snapshot);

    for (auto &input : ctx.Inputs) {
        if (input.second.Tnt.IsAnyTainted()) continue;
        m_taint.TaintByte(input.first);
    }

    ProcContext pc = SingleProcExec::Run(event, ctx, &m_taint, 0);
    pc.Dump(StdOut(), false);
}
*/

AlgorithmAnalyzer::AlgorithmAnalyzer()
{
    m_algEngine = NULL;
}
