#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_GENERIC_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_GENERIC_ANALYZER_H__
 
#include "alganalyzer.h"

struct GenericCrypto {
    Array<byte> Input, Output;
    MemRegion InputRegion, OutputRegion;
    TaintRegion MsgRegion;
    int BeginSeq, EndSeq;

    GenericCrypto(cpbyte input, cpbyte output, const MemRegion &rin,
        const MemRegion &rout, const TaintRegion &tr, int begSeq, int endSeq);
};

class GenericAnalyzer : public AlgorithmAnalyzer {
public:
    virtual ~GenericAnalyzer();
    //virtual void OnProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual bool OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual void OnComplete() override;

private:
    bool TestCrypt(const ProcContext &ctx, const MemRegion &input,
        const MemRegion &output, const TaintRegion &tr);
    std::vector<GenericCrypto *>    m_cryptos;
};

#endif // __PROPHET_PROTOCOL_ALGORITHMS_GENERIC_ANALYZER_H__