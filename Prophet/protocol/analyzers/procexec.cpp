#include "stdafx.h"
#include "procexec.h"
#include "callstack.h"
#include "protocol/taint/taintengine.h"


void ProcContext::Reset()
{
    Proc = NULL;
    BeginSeq = EndSeq = -1;
    Inputs.clear();
    Outputs.clear();
}

void ProcContext::OnTrace( ExecuteTraceEvent &event, TaintEngine *taint )
{
    const MemAccess &mr = event.Context->Mr;
    if (mr.Len != 0) {
        pbyte dat = (pbyte) &mr.Val;
        for (uint i = 0; i < mr.Len; i++)
            OnMemRead(mr.Addr+i, dat[i], taint);
    }
    const MemAccess &mw = event.Context->Mw;
    if (mw.Len != 0) {
        pbyte dat = (pbyte) &mw.Val;
        for (uint i = 0; i < mw.Len; i++)
            OnMemWrite(mw.Addr+i, dat[i], taint);
    }
}

void ProcContext::OnMemRead( u32 addr, byte val, TaintEngine *taint )
{
    if (Outputs.find(addr) != Outputs.end()) return;    // 已经被写过
    if (Inputs.find(addr) != Inputs.end()) return;      // 已经被读过
    TMemAccess tma;
    tma.Data = val;
    if (taint) {
        tma.Tnt = taint->MemTaint.Get<1>(addr)[0];
    }
    Inputs[addr] = tma;
}

void ProcContext::OnMemWrite( u32 addr, byte val, TaintEngine *taint )
{
    TMemAccess tma;
    tma.Data = val;
    if (taint) {
        tma.Tnt = taint->MemTaint.Get<1>(addr)[0];
    }
    Outputs[addr] = tma;
}

void ProcContext::Dump( File &f ) const
{
    fprintf(f.Ptr(), "Proc %08x: from %d to %d, length %d\n",
        Proc->Entry(), BeginSeq, EndSeq, EndSeq - BeginSeq + 1);
    fprintf(f.Ptr(), "Inputs:\n");
    for (auto &entry : Inputs) {
        if (!entry.second.Tnt.IsAnyTainted()) continue;
        fprintf(f.Ptr(), "  %08x : %02x  ", entry.first, entry.second.Data);
        entry.second.Tnt.Dump(f);
    }
    fprintf(f.Ptr(), "Outputs:\n");
    for (auto &entry : Outputs) {
        if (!entry.second.Tnt.IsAnyTainted()) continue;
        fprintf(f.Ptr(), "  %08x : %02x  ", entry.first, entry.second.Data);
        entry.second.Tnt.Dump(f);
    }
}


ProcExec::ProcExec(CallStack *cs, TaintEngine *te)
{
    m_callstack = cs;
    m_taint = te;

    Reset();
}

void ProcExec::Reset()
{
    //m_beginSeqs.clear();
    m_contexts.clear();
    ZeroMemory(m_workers, sizeof(m_workers));
    m_count = 0;
}

void ProcExec::OnExecuteTrace( ExecuteTraceEvent &event )
{
    if (m_contexts.empty()) {
        OnProcBegin(event);
    }
    m_contexts.back().OnTrace(event, m_taint);
}

void ProcExec::OnComplete()
{
    Reset();
}

void ProcExec::OnProcBegin( ExecuteTraceEvent &event )
{
    //m_beginSeqs.push_back(event.Seq);
    ProcContext pc;
    pc.Proc = m_callstack->Top();
    pc.BeginSeq = event.Seq;
    m_contexts.push_back(pc);
}

void ProcExec::OnProcEnd( ExecuteTraceEvent &event )
{
    Assert(m_callstack->Get().size() == m_contexts.size());
//     const ProcContext &t = m_contexts.back();
//     LxDebug("Proc %08x: %d -> %d, count(i)=%d, count(o)=%d\n", m_callstack->Top()->Entry(), 
//         t.BeginSeq, event.Seq, t.Inputs.size(), t.Outputs.size());
    m_contexts.back().EndSeq = event.Seq;
    for (int i = 0; i < m_count; i++)
        m_workers[i]->OnProcedure(event, m_contexts.back());

    m_contexts.pop_back();
}

// void ProcExec::OnMemRead( u32 addr, byte val )
// {
// 
// }
// 
// void ProcExec::OnMemWrite( u32 addr, byte val )
// {
// 
// }

void ProcExec::Add( ProcAnalyzer *analyzer )
{
    if (m_count == MaxAnalyzers) {
        LxFatal("Too many ProcAnalyzers\n");
    }

    m_workers[m_count++] = analyzer;
}

ProcDump::ProcDump( const std::string &filename )
    : m_f(filename, "w")
{

}

void ProcDump::OnProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    ctx.Dump(m_f);
}

void ProcTraceExec::RunProc( const RunTrace &t, const ProcContext &ctx )
{
    RunPartial(t, ctx.BeginSeq, ctx.EndSeq);
}

ProcContext SingleProcExec::Run(ExecuteTraceEvent &event, const ProcContext &ctx,
                                TaintEngine *taint)
{
    SingleProcExec sexec(taint);
    sexec.m_context.Proc = ctx.Proc;
    sexec.m_context.BeginSeq = ctx.BeginSeq;
    sexec.m_context.EndSeq = ctx.EndSeq;
    ProcTraceExec exec;
    if (taint) exec.Add(taint);
    exec.Add(&sexec);
    exec.RunProc(event.Trace, ctx);
    return sexec.m_context;
}

SingleProcExec::SingleProcExec( TaintEngine *te )
{
    m_taint = te;
}

void SingleProcExec::OnExecuteTrace( ExecuteTraceEvent &event )
{
    m_context.OnTrace(event, m_taint);
}
