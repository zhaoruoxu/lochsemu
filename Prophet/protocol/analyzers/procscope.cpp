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

bool Procedure::Containts( u32 addr ) const
{
    return m_addrs.find(addr) != m_addrs.end();
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
    m_entryProcs.clear();
}

void ProcScope::OnExecuteTrace( ExecuteTraceEvent &event )
{
    if (m_callStack.empty()) {
        // begin a new procedure
        OnProcBegin(event);
        m_entryProcs.insert(m_callStack.top());
    }

    Assert(!m_callStack.empty());
    m_callStack.top()->Extend(event.Context->Eip);
}

void ProcScope::OnProcBegin( ExecuteTraceEvent &event )
{
    m_callStack.push(GetProc(event.Context));
}

void ProcScope::OnProcEnd( ExecuteTraceEvent &event )
{
    Assert(!m_callStack.empty());
    m_callStack.top()->Exit(event.Context->Eip);
    m_callStack.pop();
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
}

void ProcScope::CheckValidity() const
{
    std::map<u32, Procedure *>  addrProcMap;
    for (auto &proc : m_procs) {
        if (m_entryProcs.find(proc.second) != m_entryProcs.end()) continue;
        for (auto &addr : proc.second->Addrs()) {
            auto i = addrProcMap.find(addr);
            if (i != addrProcMap.end()) {
                LxError("Procedure overlaps, addr %08x in both Procedure %08x and %08x\n",
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


