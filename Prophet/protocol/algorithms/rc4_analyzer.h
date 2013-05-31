#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__
 
#include "alganalyzer.h"

class RC4Analyzer : public AlgorithmAnalyzer {
public:
    virtual void OnOriginalProcedure(ExecuteTraceEvent &event, 
        const ProcContext &ctx) override;
    virtual void OnInputProcedure(ExecuteTraceEvent &event, 
        const ProcContext &ctx) override;
};
 
#endif // __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__