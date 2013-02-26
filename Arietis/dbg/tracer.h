#pragma once

#ifndef __ARIETIS_TRACER_H__
#define __ARIETIS_TRACER_H__

#include "Arietis.h"
#include "parallel.h"
#include "static/disassembler.h"
#include "instcontext.h"

struct TraceContext : public InstContext {
    i64     seq;
    TraceContext() : seq(-1) 
    {
    }
};

class ATracer {
public:
    typedef std::vector<TraceContext>      TraceVec;
public:
    ATracer(AEngine *engine);
    virtual ~ATracer();

    void            OnPreExecute(const Processor *cpu);
    void            OnPostExecute(const Processor *cpu, const Instruction *inst);

    //void            TraceInst(const Processor *cpu, u32 eip, i64 seq);
    void            Lock() const { m_mutex.Wait(); }
    void            Unlock() const { m_mutex.Release(); }
    void            Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool            IsEnabled() const { return m_enabled; }
    const TraceVec& GetData() const { return m_traces; }
private:
    TraceContext    m_currTrace;
    u32             m_currEip;
    i64             m_seq;
    bool            m_enabled;
    AEngine *       m_engine;
    Mutex           m_mutex;
    TraceVec        m_traces;
};

#endif // __ARIETIS_TRACER_H__
