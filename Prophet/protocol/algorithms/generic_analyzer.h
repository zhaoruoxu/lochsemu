#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_GENERIC_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_GENERIC_ANALYZER_H__
 
#include "alganalyzer.h"

struct GenericCrypto {
    Array<byte> Input, Output;
    MemRegion InputRegion, OutputRegion;
    TaintRegion MsgRegion;
    int BeginSeq, EndSeq;
    bool Ignored;

    GenericCrypto(cpbyte input, cpbyte output, const MemRegion &rin,
        const MemRegion &rout, const TaintRegion &tr, int begSeq, int endSeq);
};

class GenericSymmetricAnalyzer : public AlgorithmAnalyzer {
public:
    virtual ~GenericSymmetricAnalyzer();
    //virtual void OnProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual bool OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual void OnComplete() override;

private:
    bool TestCrypt(const ProcContext &ctx, const MemRegion &input,
        const MemRegion &output, const TaintRegion &tr);
    std::vector<GenericCrypto *>    m_cryptos;
};

enum GenericEncodingType {
    GenericEncodingOrDecoding = 0,
    GenericEncoding,
    GenericDecoding,
};

class GenericEncodingAnalyzer : public AlgorithmAnalyzer {
public:
    GenericEncodingAnalyzer(int minlen, double maxSizeDown, double maxSizeUp, double minDiffRate);
    virtual bool OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;

private:
    bool TestEncoding(const ProcContext &ctx, const MemRegion &input,
        const MemRegion &output, const TaintRegion &tr, const Taint &tin);
private:
    static std::string EncodingTypeStr[];
private:
    int     m_minlen;
    double  m_maxSizeDown, m_maxSizeUp, m_minDiffRate;
};

class GenericChecksumAnalyzer : public AlgorithmAnalyzer {
public:
    virtual bool OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;

private:
    bool CheckRegisterChecksum(ExecuteTraceEvent &event, const ProcContext &ctx, 
        const MemRegion &input, const TaintRegion &tr, const Taint &tin);
};

#endif // __PROPHET_PROTOCOL_ALGORITHMS_GENERIC_ANALYZER_H__