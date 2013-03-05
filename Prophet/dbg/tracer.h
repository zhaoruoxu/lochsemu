#pragma once

#ifndef __PROPHET_TRACER_H__
#define __PROPHET_TRACER_H__

#include "Prophet.h"
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

class ProTracer : public MutexSyncObject, public ISerializable {
public:
    typedef std::vector<TraceContext>      TraceVec;
public:
    ProTracer(ProEngine *engine);
    virtual ~ProTracer();

    void            OnPreExecute(PreExecuteEvent &event);
    void            OnPostExecute(PostExecuteEvent &event);

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
    ProEngine *     m_engine;
    TraceVec        m_traces;
};

#endif // __PROPHET_TRACER_H__
