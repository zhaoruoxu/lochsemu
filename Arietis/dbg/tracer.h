#pragma once

#ifndef __ARIETIS_TRACER_H__
#define __ARIETIS_TRACER_H__

#include "Arietis.h"
#include "parallel.h"
#include "static/disassembler.h"
#include "instcontext.h"
#include "utilities.h"

struct TraceContext : public InstContext {
    i64     seq;
    TraceContext() : seq(-1) 
    {
    }
};

class ATracer : public MutexSyncObject, public ISerializable {
public:
    typedef std::vector<TraceContext>      TraceVec;
public:
    ATracer(AEngine *engine);
    virtual ~ATracer();

    void            OnPreExecute(const Processor *cpu);
    void            OnPostExecute(const Processor *cpu, const Instruction *inst);

    //void            TraceInst(const Processor *cpu, u32 eip, i64 seq);
    void            Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool            IsEnabled() const { return m_enabled; }
    const TraceVec& GetData() const { return m_traces; }

    void            Serialize(Json::Value &root) const override;
    void            Deserialize(Json::Value &root) override;
private:
    TraceContext    m_currTrace;
    u32             m_currEip;
    i64             m_seq;
    bool            m_enabled;
    AEngine *       m_engine;
    TraceVec        m_traces;
};

#endif // __ARIETIS_TRACER_H__
