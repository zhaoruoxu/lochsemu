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
    const MemAccess &mr = event.Context->Mr;
    if (mr.Len != 0) {
        pbyte dat = (pbyte) &mr.Val;
        for (uint i = 0; i < mr.Len; i++)
            OnMemRead(mr.Addr+i, dat[i]);
    }
    const MemAccess &mw = event.Context->Mw;
    if (mw.Len != 0) {
        pbyte dat = (pbyte) &mw.Val;
        for (uint i = 0; i < mw.Len; i++)
            OnMemWrite(mw.Addr+i, dat[i]);
    }
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
        m_workers[i]->OnProcedure(m_contexts.back());

    m_contexts.pop_back();
}

void ProcExec::OnMemRead( u32 addr, byte val )
{
    ProcContext &t = m_contexts.back();
    if (t.Outputs.find(addr) != t.Outputs.end()) return;    // 已经被写过
    if (t.Inputs.find(addr) != t.Inputs.end()) return;      // 已经被读过
    TMemAccess tma;
    tma.Data = val;
    tma.Tnt = m_taint->MemTaint.Get<1>(addr)[0];
    t.Inputs[addr] = tma;
}

void ProcExec::OnMemWrite( u32 addr, byte val )
{
    TMemAccess tma;
    tma.Data = val;
    tma.Tnt = m_taint->MemTaint.Get<1>(addr)[0];
    m_contexts.back().Outputs[addr] = tma;
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

void ProcDump::OnProcedure( const ProcContext &ctx )
{
    fprintf(m_f.Ptr(), "Proc %08x: from %d to %d, length %d\n",
        ctx.Proc->Entry(), ctx.BeginSeq, ctx.EndSeq, ctx.EndSeq - ctx.BeginSeq + 1);
    fprintf(m_f.Ptr(), "Inputs:\n");
    for (auto &entry : ctx.Inputs) {
        if (!entry.second.Tnt.IsAnyTainted()) continue;
        fprintf(m_f.Ptr(), "  %08x : %02x  ", entry.first, entry.second.Data);
        entry.second.Tnt.Dump(m_f);
    }
    fprintf(m_f.Ptr(), "Outputs:\n");
    for (auto &entry : ctx.Outputs) {
        if (!entry.second.Tnt.IsAnyTainted()) continue;
        fprintf(m_f.Ptr(), "  %08x : %02x  ", entry.first, entry.second.Data);
        entry.second.Tnt.Dump(m_f);
    }
}
