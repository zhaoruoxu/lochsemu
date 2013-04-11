#include "stdafx.h"
#include "tracer.h"
#include "instruction.h"
#include "processor.h"
#include "engine.h"
#include "event.h"

ProTracer::ProTracer( ProEngine *engine )
    : m_engine(engine),  m_seq(-1)
{
    m_mainModuleOnly = false;
    m_traces    = NULL;
    m_maxTraces = 100000;
    m_ptr       = 0;
    m_count     = 0;
}

ProTracer::~ProTracer()
{
    SAFE_DELETE_ARRAY(m_traces);
}

void ProTracer::OnProcessPostLoad( ProcessPostLoadEvent &event )
{
    LxDebug("Allocating spaces for %d traces\n", m_maxTraces);
    m_traces = new TraceContext[m_maxTraces];
}

void ProTracer::OnPreExecute( PreExecuteEvent &event )
{
    //m_currEip   = event.Cpu->EIP;
    if (!IsEnabled()) return;
    m_currTrace.Reset();
}

void ProTracer::OnPostExecute( PostExecuteEvent &event )
{
    ++m_seq;

    if (!IsEnabled()) return;
    if (m_mainModuleOnly && event.Cpu->GetCurrentModule() != 0)
        return; // skip DLLs

    m_engine->GetTraceContext(event.Cpu, &m_currTrace, event.Cpu->GetPrevEip());
    m_currTrace.Seq     = m_seq;
    AddTrace(m_currTrace);
}

void ProTracer::AddTrace( const TraceContext &t )
{
    SyncObjectLock lock(*this);
    m_traces[m_ptr++] = t;
    if (m_ptr >= m_maxTraces)
        m_ptr = 0;
    if (m_count < m_maxTraces)
        m_count++;
}

const TraceContext & ProTracer::GetTrace( int n ) const
{
    Assert(n >= 0 && n < m_count);
    int p = m_ptr + n - m_count;
    if (p < 0) p += m_maxTraces;
    return m_traces[p];
}

void ProTracer::Serialize( Json::Value &root ) const 
{
    root["enabled"]             = m_enabled;
    root["main_module_only"]    = m_mainModuleOnly;
    root["max_traces"]          = m_maxTraces;
}

void ProTracer::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();
    m_mainModuleOnly = root.get("main_module_only", m_mainModuleOnly).asBool();
    m_maxTraces = root.get("max_traces", m_maxTraces).asInt();
}

void ProTracer::Enable( bool isEnabled )
{
    m_enabled = isEnabled;
    m_engine->UpdateGUI();
}

int ProTracer::FindFirstReg( u32 val ) const
{
    for (int i = 0; i < m_count; i++) {
        const TraceContext &t = GetTrace(i);
        for (int r = 0; r < t.RegCount; r++) {
            if (t.Regs[r] == val) return i;
        }
        for (auto &mr : t.MRs)
            if (mr.Len == 4 && mr.Val == val) return i;
        for (auto &mw : t.MWs)
            if (mw.Len == 4 && mw.Val == val) return i;
    }
    return -1;
}

int ProTracer::FindMostRecentMrAddr( u32 addr, int idxFrom ) const
{
    int index = idxFrom - 1;
    while (index >= 0) {
        for (auto &mr : GetTrace(index).MRs) {
            if (mr.Addr == addr) return index;
        }
        index--;
    }
    return -1;
}

int ProTracer::FindMostRecentMwAddr( u32 addr, int idxFrom ) const
{
    int index = idxFrom - 1;
    while (index >= 0) {
        for (auto &mw : GetTrace(index).MWs) {
            if (mw.Addr == addr) return index;
        }
        index--;
    }
    return -1;
}


