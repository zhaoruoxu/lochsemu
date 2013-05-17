#include "stdafx.h"
#include "runtrace.h"
#include "protocol.h"

RunTracer::RunTracer(Protocol *engine) : m_engine(engine)
{
    m_count = 0;
    m_maxTraces = 100000;
    m_traces = NULL;
}

RunTracer::~RunTracer()
{
    SAFE_DELETE_ARRAY(m_traces);
}

void RunTracer::Trace( const Processor *cpu )
{
    m_engine->UpdateTContext(cpu, m_traces + m_count);
    m_count++;
}

void RunTracer::Begin()
{
    LxInfo("Allocating %d traces for RunTracer, Memory used = %d KB\n", 
        m_maxTraces, m_maxTraces * sizeof(TContext) / 1024);
    Assert(m_traces == NULL);
    m_traces = new TContext[m_maxTraces];
    if (m_traces == NULL) {
        LxFatal("Error allocating traces for RunTracer\n");
    }
    ZeroMemory(m_traces, sizeof(TContext) * m_maxTraces);
    m_count = 0;
}

void RunTracer::End()
{
    Assert(m_traces != NULL);
    SAFE_DELETE_ARRAY(m_traces);
    m_count = 0;
}

void RunTracer::Serialize( Json::Value &root ) const 
{
    root["max_traces"] = m_maxTraces;
}

void RunTracer::Deserialize( Json::Value &root )
{
    m_maxTraces = root.get("max_traces", m_maxTraces).asInt();
}
