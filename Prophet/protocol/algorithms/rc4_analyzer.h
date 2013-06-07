#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__
 
#include "alganalyzer.h"
#include "memregion.h"

struct RC4Context {
    static const u32 SboxLength = 256;

    byte Key[256];
    byte Sbox[SboxLength];
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
    static const u32 SboxLength = RC4Context::SboxLength;
private:
    void TestKeySchedule(const ProcContext &ctx, const MemRegion &region);
    void TestKeySchedule(const ProcContext &ctx, u32 sboxAddr);
    void TestRC4Crypt(const ProcContext &ctx, const MemRegion &region);
    void TestRC4Crypt(const ProcContext &ctx, const MemRegion &input, 
        const MemRegion &output, const TaintRegion &tin);

private:
    std::vector<RC4Context> m_contexts;
};
 
#endif // __PROPHET_PROTOCOL_ALGORITHMS_RC4_ANALYZER_H__