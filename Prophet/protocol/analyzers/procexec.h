#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_PROCEXEC_H__
#define __PROPHET_PROTOCOL_ANALYZERS_PROCEXEC_H__
 
#include "protocol/analyzer.h"
#include "procscope.h"
#include "traceexec.h"

struct TMemAccess {
    byte Data;
    Taint Tnt;

};

typedef std::map<u32, TMemAccess> ProcParameter;

std::vector<MemRegion>  GenerateMemRegions(const ProcParameter &params);
void FillMemRegionBytes(const ProcParameter &params, const MemRegion &r, pbyte dest);
Taint GetMemRegionTaintAnd(const ProcParameter &params, const MemRegion &r);
Taint GetMemRegionTaintOr(const ProcParameter &params, const MemRegion &r);

struct ProcContext {
    Procedure *Proc;
    int BeginSeq;
    int EndSeq;
    ProcParameter Inputs, Outputs;
    std::vector<MemRegion> InputRegions, OutputRegions;
    Taint TRegs[8];
    Taint TEip;

    int Count;
    int Level;

    void Reset();
    ProcContext() {
        Reset();
    }

    void OnTrace(ExecuteTraceEvent &event, TaintEngine *taint);
    void Dump(File &f, bool taintedOnly) const;
    void GenerateRegions();
    void GetRegTaints(TaintEngine *te);

private:
    void OnMemRead(u32 addr, byte val, TaintEngine *taint);
    void OnMemWrite(u32 addr, byte val, TaintEngine *taint);
};

class ProcAnalyzer {
public:
    virtual ~ProcAnalyzer() {}
    virtual void OnProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) = 0;
    virtual void OnComplete() = 0;
};

class ProcExec : public TraceAnalyzer {
public:
    ProcExec(CallStack *cs, TaintEngine *te);
    
    virtual void Reset() override;
    virtual void OnExecuteTrace(ExecuteTraceEvent &event) override;
    virtual void OnComplete() override;
    virtual void OnProcBegin(ExecuteTraceEvent &event) override;
    virtual void OnProcEnd(ExecuteTraceEvent &event) override;

    void Add(ProcAnalyzer *analyzer);
// private:
//     void    OnMemRead(u32 addr, byte val);
//     void    OnMemWrite(u32 addr, byte val);

private:
    //std::vector<int>    m_beginSeqs;
    CallStack *         m_callstack;
    TaintEngine *       m_taint;
    std::vector<ProcContext>    m_contexts;

    static const int MaxAnalyzers = 16;
    ProcAnalyzer * m_workers[MaxAnalyzers];
    int m_count;
};

class ProcTraceExec : public TraceExec {
public:
    ProcTraceExec(const RunTrace &t) : TraceExec(t) {}
    void RunProc(const ProcContext &ctx);
};

class SingleProcExec : public TraceAnalyzer {
public:
    static ProcContext Run(ExecuteTraceEvent &event, const ProcContext &ctx, 
        TaintEngine *taint);
private:
    SingleProcExec(TaintEngine *te, const ProcContext &ctx);
    virtual void OnExecuteTrace(ExecuteTraceEvent &event) override;
    
private:
    TaintEngine *m_taint;
    ProcContext m_context;
};

class ProcDump : public ProcAnalyzer {
public:
    ProcDump(const std::string &filename);

    virtual void OnProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
private:
    File m_f;
};

#endif // __PROPHET_PROTOCOL_ANALYZERS_PROCEXEC_H__