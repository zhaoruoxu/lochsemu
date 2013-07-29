#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_BASE64_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_BASE64_ANALYZER_H__

#include "alganalyzer.h"

class Base64Analyzer : public AlgorithmAnalyzer {
public:
    Base64Analyzer();
    virtual bool OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;

private:
    bool TestBase64(const ProcContext &ctx, const MemRegion &input, 
        const MemRegion &output, const TaintRegion &tr);
};

#endif // __PROPHET_PROTOCOL_ALGORITHMS_BASE64_ANALYZER_H__