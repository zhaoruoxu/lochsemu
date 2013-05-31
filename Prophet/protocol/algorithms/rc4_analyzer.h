#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__
 
#include "alganalyzer.h"

struct RC4Context {
    byte Key[256];
    MemRegion KeyRegion;
    MemRegion SboxRegion;

    bool TryKeySchedule(const ProcContext &ctx, const MemRegion &key, const MemRegion &sbox);
};

class RC4Analyzer : public AlgorithmAnalyzer {
public:
    virtual void OnOriginalProcedure(ExecuteTraceEvent &event, 
        const ProcContext &ctx) override;
    virtual void OnInputProcedure(ExecuteTraceEvent &event, 
        const ProcContext &ctx) override;

public:
    static const u32 SboxLength = 256;
private:
    void TestKeySchedule(const ProcContext &ctx, const MemRegion &region);
    void TestKeySchedule(const ProcContext &ctx, u32 sboxAddr);

private:
    std::vector<RC4Context> m_contexts;
};
 
#endif // __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__