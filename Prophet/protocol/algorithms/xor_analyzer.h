#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_XOR_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_XOR_ANALYZER_H__
 
#include "alganalyzer.h"

class ChainedXorAnalyzer : public AlgorithmAnalyzer {
public:
    virtual void OnProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual void OnInputProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual void OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;

private:
    void TestCrypt(const ProcContext &ctx, const MemRegion &input, 
        const MemRegion &output, const TaintRegion &tr);
};
 
#endif // __PROPHET_PROTOCOL_ALGORITHMS_XOR_ANALYZER_H__