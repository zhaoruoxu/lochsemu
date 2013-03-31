#pragma once

#ifndef __PROPHET_TRACER_H__
#define __PROPHET_TRACER_H__

#include "prophet.h"
#include "parallel.h"
#include "static/disassembler.h"
#include "instcontext.h"
#include "utilities.h"

struct TraceContext : public InstContext {
    i64     Seq;

    TraceContext() : Seq(-1) 
    {
    }

    void    Reset() {
        InstContext::Reset();
        Seq = 0;
    }
};


class ProTracer : public MutexSyncObject, public ISerializable {
public:
    ProTracer(ProEngine *engine);
    virtual ~ProTracer();

    void            OnProcessPostLoad(ProcessPostLoadEvent &event);
    void            OnPreExecute(PreExecuteEvent &event);
    void            OnPostExecute(PostExecuteEvent &event);

    int             GetCount() const { return m_count; }
    const TraceContext &    GetTrace(int n) const;

    int             FindFirstReg(u32 val) const;
    int             FindMostRecentMrAddr(u32 addr, int idxFrom) const;
    int             FindMostRecentMwAddr(u32 addr, int idxFrom) const;

    void            Enable(bool isEnabled);
    bool            IsEnabled() const { return m_enabled; }

    void            Serialize(Json::Value &root) const override;
    void            Deserialize(Json::Value &root) override;
private:
    void            AddTrace(const TraceContext &t);
private:
    TraceContext    m_currTrace;
    //u32             m_currEip;
    i64             m_seq;
    bool            m_enabled;
    bool            m_mainModuleOnly;
    ProEngine *     m_engine;
    int             m_maxTraces;
    TraceContext *  m_traces;
    int             m_ptr;
    int             m_count;
};

#endif // __PROPHET_TRACER_H__
