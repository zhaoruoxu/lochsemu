#include "stdafx.h"
#include "tracer.h"
#include "instruction.h"
#include "processor.h"
#include "engine.h"
#include "event.h"

ATracer::ATracer( AEngine *engine )
    : m_engine(engine),  m_seq(-1)
{
}

ATracer::~ATracer()
{
}

void ATracer::OnPreExecute( PreExecuteEvent &event )
{
    m_currEip   = event.Cpu->EIP;
    if (!IsEnabled()) return;
}

void ATracer::OnPostExecute( PostExecuteEvent &event )
{
    ++m_seq;

    if (!IsEnabled()) return;

    TraceContext ctx;
    m_engine->GetTraceContext(&ctx, m_currEip);
    ctx.seq     = m_seq;

    {
        SyncObjectLock lock(*this);
        m_traces.push_back(ctx);
    }
}

void ATracer::Serialize( Json::Value &root ) const 
{
    root["enabled"] = m_enabled;
}

void ATracer::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();
}
