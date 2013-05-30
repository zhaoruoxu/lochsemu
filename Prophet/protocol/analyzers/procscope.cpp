#include "stdafx.h"
#include "procscope.h"
#include "processor.h"

void Procedure::Dump( File &f ) const
{
    fprintf(f.Ptr(), "Procedure %08x\n", m_entry);
    for (auto &t : m_addrs) {
        if (m_exits.find(t) != m_exits.end())
            fprintf(f.Ptr(), "\t%08x(exit)\n", t);
        else
            fprintf(f.Ptr(), "\t%08x\n", t);
    }
    fprintf(f.Ptr(), "\n");
}

void Procedure::Exit( u32 addr )
{
    Assert(m_addrs.find(addr) != m_addrs.end());
    m_exits.insert(addr);
}

ProcScope::ProcScope()
{
    Reset();
}

ProcScope::~ProcScope()
{
    Reset();
}

void ProcScope::Reset()
{
    for (auto &entry : m_procs) {
        SAFE_DELETE(entry.second);
    }
    m_procs.clear();
    //m_postOrder.clear();
    // m_prev = NULL;
}

void ProcScope::OnExecuteTrace( ExecuteTraceEvent &event )
{
    // const TContext *ctx = event.Context;

//     if (m_callStack.empty() ||
//         (m_prev && Instruction::IsCall(m_prev->Inst) && 
//         !m_prev->HasExecFlag(LX_EXEC_WINAPI_CALL) && 
//         !m_prev->HasExecFlag(LX_EXEC_WINAPI_JMP)) ) 
//     {
// 
//     }

    if (m_callStack.empty()) {
        // begin a new procedure
        OnProcBegin(event);
    }

    Assert(!m_callStack.empty());
    m_callStack.top()->Extend(event.Context->Eip);

//     if (Instruction::IsRet(ctx->Inst)) {
// 
//     }
    // m_prev = ctx;
    // m_prevSeq = event.Seq;
}

void ProcScope::OnProcBegin( ExecuteTraceEvent &event )
{
    m_callStack.push(GetProc(event.Context));
    //m_beginSeqs.push(event.Seq);
}

void ProcScope::OnProcEnd( ExecuteTraceEvent &event )
{
    m_callStack.top()->Exit(event.Context->Eip);
    //m_postOrder.emplace_back(m_callStack.top(), m_beginSeqs.top(), event.Seq);
    m_callStack.pop();
    //m_beginSeqs.pop();
    // Assert(m_callStack.size() == m_beginSeqs.size());
}

void ProcScope::OnComplete()
{
#ifdef _DEBUG
    CheckValidity();
#endif
}

Procedure * ProcScope::GetProc( const TContext *entry )
{
    u32 eip = entry->Eip;
    auto iter = m_procs.find(eip);
    if (iter != m_procs.end())
        return iter->second;
    Procedure *r = new Procedure(eip);
    m_procs[eip] = r;
    return r;
}

void ProcScope::Dump( File &f ) const
{
    for (auto &entry : m_procs) {
        entry.second->Dump(f);
    }

//     fprintf(f.Ptr(), "\nPost order execution history:\n");
//     for (auto &entry : m_postOrder) {
//         fprintf(f.Ptr(), "Proc %08x: from %d to %d, length %d\n", 
//             entry.Proc->Entry(), entry.BeginSeq, entry.EndSeq, 
//             entry.EndSeq - entry.BeginSeq + 1);
//     }
}


void ProcScope::CheckValidity() const
{
    std::map<u32, Procedure *>  addrProcMap;
    for (auto &proc : m_procs) {
        for (auto &addr : proc.second->Addrs()) {
            auto i = addrProcMap.find(addr);
            if (i != addrProcMap.end()) {
                LxFatal("Procedure overlaps, addr %08x in both Procedure %08x and %08x\n",
                    addr, i->second->Entry(), proc.second->Entry());
            }
            addrProcMap[addr] = proc.second;
        }
    }

    LxDebug("ProcScope validity okay\n");
}

Procedure * ProcScope::Get( u32 entry ) const
{
    auto i = m_procs.find(entry);
    if (i == m_procs.end()) return NULL;
    return i->second;
}


