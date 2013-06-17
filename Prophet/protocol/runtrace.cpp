#include "stdafx.h"
#include "runtrace.h"
#include "protocol.h"
#include "processor.h"
#include "message.h"

RunTrace::RunTrace(Protocol *engine) : m_engine(engine)
{
    m_count = 0;
    m_maxTraces = 100000;
    m_traces = NULL;
    m_mergeCallJmp = true;
}

RunTrace::~RunTrace()
{
    SAFE_DELETE_ARRAY(m_traces);
}

void RunTrace::Trace( const Processor *cpu )
{
    m_engine->UpdateTContext(cpu, m_traces + m_count);

    if (m_mergeCallJmp && m_count > 0 &&
        Instruction::IsIndirectJump(m_traces[m_count].Inst) &&
        Instruction::IsCall(m_traces[m_count-1].Inst)
        ) 
    {
        LxDebug("CALL-JMP ignored, [%08x] %s\n", 
            m_traces[m_count].Eip, m_traces[m_count].Inst->Main.CompleteInstr);
        m_traces[m_count-1].ExecFlag |= m_traces[m_count].ExecFlag; // merge exec flag
        return;
    }

    m_count++;
}

void RunTrace::Begin()
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

void RunTrace::End()
{
    Assert(m_traces != NULL);
    SAFE_DELETE_ARRAY(m_traces);
    m_count = 0;
}

void RunTrace::Serialize( Json::Value &root ) const 
{
    root["max_traces"] = m_maxTraces;
    root["merge_calljmp"] = m_mergeCallJmp;
}

void RunTrace::Deserialize( Json::Value &root )
{
    m_maxTraces = root.get("max_traces", m_maxTraces).asInt();
    m_mergeCallJmp = root.get("merge_calljmp", m_mergeCallJmp).asBool();
}

void RunTrace::Dump( File &f ) const
{
    for (int i = 0; i < m_count; i++)
        m_traces[i].Dump(f);
}

void RunTrace::DumpMsg( Message *msg, File &f ) const
{
    Assert(msg->GetTraceEnd() < m_count);
    for (int i = msg->GetTraceBegin(); i <= msg->GetTraceEnd(); i++)
        m_traces[i].Dump(f);
}

void TContext::Dump( File &f ) const
{
    fprintf(f.Ptr(), "%08x  %-40s \tEsp=%08x  EF=%x",
        Eip, Inst->Main.CompleteInstr, Regs[LX_REG_ESP], ExecFlag);
    if (Mr.Addr) fprintf(f.Ptr(), "  R[%x:%d]%08x", Mr.Addr, Mr.Len, Mr.Val);
    if (Mw.Addr) fprintf(f.Ptr(), "  W[%x:%d]%08x", Mw.Addr, Mw.Len, Mw.Val);
    if (JumpTaken) fprintf(f.Ptr(), "  JUMP");
    fprintf(f.Ptr(), "\n");
}
