#pragma once

#ifndef __ARIETIS_TRACER_H__
#define __ARIETIS_TRACER_H__

#include "Arietis.h"
#include "parallel.h"
#include "static/disassembler.h"

class ATracer {
public:
    struct Trace {
        static const int RegCount = 8;
        u32     regs[RegCount];
        InstPtr inst;
        i64     seq;

        Trace(): seq(-1), inst(NULL) {}
        Trace(InstPtr i, const Processor *cpu, i64 seq);

//         Trace(const Trace &t);
//         Trace &operator=(const Trace &t);
    };

    typedef std::vector<Trace>      TraceVec;
public:
    ATracer(AEngine *engine);
    virtual ~ATracer();
    void            TraceInst(const Processor *cpu, u32 eip, i64 seq);
    void            Lock() const { m_mutex.Wait(); }
    void            Unlock() const { m_mutex.Release(); }
    const TraceVec& GetData() const { return m_traces; }
private:
    AEngine *       m_engine;
    Mutex           m_mutex;
    TraceVec        m_traces;
};

#endif // __ARIETIS_TRACER_H__
