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
}

ProTracer::~ProTracer()
{
}

void ProTracer::OnPreExecute( PreExecuteEvent &event )
{
    m_currEip   = event.Cpu->EIP;
    if (!IsEnabled()) return;
}

void ProTracer::OnPostExecute( PostExecuteEvent &event )
{
    ++m_seq;

    if (!IsEnabled()) return;
    if (m_mainModuleOnly && event.Cpu->GetCurrentModule() != 0)
        return; // skip DLLs

    TraceContext ctx;
    m_engine->GetTraceContext(&ctx, m_currEip);
    ctx.seq     = m_seq;

    {
        SyncObjectLock lock(*this);
        m_traces.push_back(ctx);
    }
}

void ProTracer::Serialize( Json::Value &root ) const 
{
    root["enabled"]             = m_enabled;
    root["main_module_only"]    = m_mainModuleOnly;
}

void ProTracer::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();
    m_mainModuleOnly = root.get("main_module_only", m_mainModuleOnly).asBool();
}

void ProTracer::Enable( bool isEnabled )
{
    m_enabled = isEnabled;
    m_engine->UpdateGUI();
}
