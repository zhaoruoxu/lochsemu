#include "stdafx.h"
#include "tracer.h"
#include "instruction.h"
#include "processor.h"
#include "engine.h"

ATracer::ATracer( AEngine *engine )
    : m_engine(engine), m_mutex(false), m_seq(-1)
{
    
}

ATracer::~ATracer()
{
}

void ATracer::OnPreExecute( const Processor *cpu )
{
    m_currEip   = cpu->EIP;
    if (!m_enabled) return;

    
}

void ATracer::OnPostExecute( const Processor *cpu, const Instruction *inst )
{
    ++m_seq;

    if (!m_enabled) return;

    TraceContext ctx;

    Lock();
    m_engine->GetTraceContext(&ctx, m_currEip);
    //ctx.regs[InstContext::RegIndexEip] = m_currEip;     // Fix eip error
    ctx.seq     = m_seq;
    m_traces.push_back(ctx);
    Unlock();
}

