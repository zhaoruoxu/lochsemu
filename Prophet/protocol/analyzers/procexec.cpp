#include "stdafx.h"
#include "procexec.h"
#include "callstack.h"
#include "protocol/taint/taintengine.h"
#include "memregion.h"

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

    for (int i = 0; i < 8; i++) {
        if (!TRegs[i].IsAnyTainted()) continue;
        fprintf(f.Ptr(), "%s: ", InstContext::RegNames[i].c_str());
        TRegs[i].Dump(f);
    }
    if (TEip.IsAnyTainted()) {
        fprintf(f.Ptr(), "Eip: ");
        TEip.Dump(f);
    }

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

void ProcContext::GetRegTaints( TaintEngine *te )
{
    TEip = te->CpuTaint.Eip[0];
    for (int i = 0; i < 8; i++)
        TRegs[i] = Shrink(te->CpuTaint.GPRegs[i])[0];
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
    for (int i = 0; i < m_count; i++)
        m_workers[i]->OnComplete();
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
    back.GetRegTaints(m_taint);
    
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
                                TaintEngine *taint)
{
    SingleProcExec sexec(taint, ctx);
    ProcTraceExec exec(event.Trace);
    if (taint) exec.Add(taint);
    exec.Add(&sexec);
    exec.RunProc(ctx);
    sexec.m_context.GenerateRegions();
    sexec.m_context.GetRegTaints(taint);
    return sexec.m_context;
}

SingleProcExec::SingleProcExec( TaintEngine *te, const ProcContext &ctx )
{
    m_taint = te;
    m_context.Proc = ctx.Proc;
    m_context.BeginSeq = ctx.BeginSeq;
    m_context.EndSeq = ctx.EndSeq;
    m_context.Level = ctx.Level;
}

void SingleProcExec::OnExecuteTrace( ExecuteTraceEvent &event )
{
    m_context.OnTrace(event, m_taint);
}

#define SEPARATE_MEMREGION_BY_TAINT 1

std::vector<MemRegion> GenerateMemRegions( const ProcParameter &params )
{
    std::vector<MemRegion> r;
    if (params.empty()) return r;
    auto iter = params.begin();
    MemRegion curr(iter->first, 1);
    u32 prev = iter->first;
    bool prevTainted = iter->second.Tnt.IsAnyTainted();
    for (iter++; iter != params.end(); iter++) {
#if SEPARATE_MEMREGION_BY_TAINT
        if (iter->first == prev + 1 && iter->second.Tnt.IsAnyTainted() == prevTainted) {
#else
        if (iter->first == prev + 1) {
#endif
            prev++;
        } else {
            curr.Len = prev - curr.Addr + 1;
            r.push_back(curr);
            curr.Addr = iter->first; curr.Len = 1;
            prev = iter->first;
            prevTainted = iter->second.Tnt.IsAnyTainted();
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

Taint GetMemRegionTaintAnd( const ProcParameter &params, const MemRegion &r )
{
    Taint t;
    t.SetAll();
    for (u32 i = 0; i < r.Len; i++)
        t &= params.find(r.Addr + i)->second.Tnt;
    return t;
}

Taint GetMemRegionTaintOr( const ProcParameter &params, const MemRegion &r )
{
    Taint t;
    for (u32 i = 0; i < r.Len; i++)
        t |= params.find(r.Addr + i)->second.Tnt;
    return t;
}
