#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_HASH_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_HASH_ANALYZER_H__
 
#include "alganalyzer.h"

class MD5Analyzer : public AlgorithmAnalyzer {
public:
    virtual bool OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;

public:
    static const int MDSize = 16;

private:
    bool TestMD5(const ProcContext &ctx, const MemRegion &input, 
        const MemRegion &output, const TaintRegion &tr);
};
 
#endif // __PROPHET_PROTOCOL_ALGORITHMS_HASH_ANALYZER_H__