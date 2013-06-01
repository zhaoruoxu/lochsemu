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
    Count = 0;
    Level = 0;
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
    Count++;
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

void ProcContext::Dump( File &f, bool taintedOnly ) const
{
    fprintf(f.Ptr(), "Proc %08x: from %d to %d, length %d, count %d, level %d\n",
        Proc->Entry(), BeginSeq, EndSeq, EndSeq - BeginSeq + 1, Count, Level);

    std::vector<MemRegion> inputRegions = GenerateMemRegions(Inputs),
        outputRegions = GenerateMemRegions(Outputs);
    fprintf(f.Ptr(), "Inputs:");
    for (auto &region : inputRegions)
        fprintf(f.Ptr(), " (%08x-%08x:%d)", region.Addr, 
        region.Addr + region.Len - 1, region.Len);
    fprintf(f.Ptr(), "\n");
    for (auto &entry : Inputs) {
        if (taintedOnly && !entry.second.Tnt.IsAnyTainted()) continue;
        fprintf(f.Ptr(), "  %08x : %02x  ", entry.first, entry.second.Data);
        entry.second.Tnt.Dump(f);
    }
    fprintf(f.Ptr(), "Outputs:");
    for (auto &region : outputRegions)
        fprintf(f.Ptr(), " (%08x-%08x:%d)", region.Addr, 
        region.Addr + region.Len - 1, region.Len);
    fprintf(f.Ptr(), "\n");
    for (auto &entry : Outputs) {
        if (taintedOnly && !entry.second.Tnt.IsAnyTainted()) continue;
        fprintf(f.Ptr(), "  %08x : %02x  ", entry.first, entry.second.Data);
        entry.second.Tnt.Dump(f);
    }
}

void ProcContext::GenerateRegions()
{
    InputRegions = GenerateMemRegions(Inputs);
    OutputRegions = GenerateMemRegions(Outputs);
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
    ProcContext pc;
    pc.Proc = m_callstack->Top();
    pc.BeginSeq = event.Seq;
    m_contexts.push_back(pc);
}

void ProcExec::OnProcEnd( ExecuteTraceEvent &event )
{
    Assert(m_callstack->Get().size() == m_contexts.size());
    auto back = m_contexts.back();
    m_contexts.pop_back();
    back.EndSeq = event.Seq;
    back.GenerateRegions();
    for (int i = 0; i < m_count; i++)
        m_workers[i]->OnProcedure(event, back);
    if (!m_contexts.empty()) {
        m_contexts.back().Level = max(m_contexts.back().Level, back.Level + 1);
    }
}

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
    ctx.Dump(m_f, false);
}

void ProcTraceExec::RunProc( const ProcContext &ctx )
{
    RunPartial(ctx.BeginSeq, ctx.EndSeq);
}

ProcContext SingleProcExec::Run(ExecuteTraceEvent &event, const ProcContext &ctx,
                                TaintEngine *taint, int maxDepth)
{
    SingleProcExec sexec(taint, ctx, maxDepth);
    ProcTraceExec exec(event.Trace);
    if (taint) exec.Add(taint);
    exec.Add(&sexec);
    exec.RunProc(ctx);
    sexec.m_context.GenerateRegions();
    return sexec.m_context;
}

SingleProcExec::SingleProcExec( TaintEngine *te, const ProcContext &ctx, int maxDepth )
{
    m_taint = te;
    m_context.Proc = ctx.Proc;
    m_context.BeginSeq = ctx.BeginSeq;
    m_context.EndSeq = ctx.EndSeq;
    m_context.Level = ctx.Level;
    m_maxDepth = maxDepth;
}

void SingleProcExec::OnExecuteTrace( ExecuteTraceEvent &event )
{
//     if (m_maxDepth > 0) {
//         LxWarning("SingleProcExec depth > 0 not supported\n");
//     }
    if (m_context.Level <= m_maxDepth) {
        m_context.OnTrace(event, m_taint);
    }
}

std::vector<MemRegion> GenerateMemRegions( const ProcParameter &params )
{
    std::vector<MemRegion> r;
    if (params.empty()) return r;
    auto iter = params.begin();
    MemRegion curr(iter->first, 1);
    u32 prev = iter->first;
    for (iter++; iter != params.end(); iter++) {
        if (iter->first == prev + 1) {
            prev++;
        } else {
            curr.Len = prev - curr.Addr + 1;
            r.push_back(curr);
            curr.Addr = iter->first; curr.Len = 1;
            prev = iter->first;
        }
    }
    curr.Len = prev - curr.Addr + 1;
    r.push_back(curr);
    return r;
}

void FillMemRegionBytes( const ProcParameter &params, const MemRegion &r, pbyte dest )
{
    for (u32 i = 0; i < r.Len; i++) {
        dest[i] = params.find(r.Addr + i)->second.Data;
    }
}
