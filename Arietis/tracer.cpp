#include "stdafx.h"
#include "tracer.h"
#include "instruction.h"
#include "processor.h"
#include "engine.h"

ATracer::ATracer( AEngine *engine )
    : m_engine(engine), m_mutex(false)
{
    
}

ATracer::~ATracer()
{
}

void ATracer::TraceInst( const Processor *cpu, u32 eip, i64 seq )
{
    m_mutex.Wait();
    //LxInfo("Trace instruction: %08X  %s\n", cpu->EIP, inst->Main.CompleteInstr);

    m_traces.emplace_back(m_engine->GetDisassembler()->GetInst(cpu, eip), cpu, seq);
    m_mutex.Release();
}

ATracer::Trace::Trace( const Disassembler::Inst &i, const Processor *cpu, i64 s )
{
    inst = i;
    seq = s;
    for (int n = 0; n < Trace::RegCount; n++)
        regs[n] = cpu->GP_Regs[n].X32;
}
