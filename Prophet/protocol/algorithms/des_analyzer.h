#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_DES_ANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_DES_ANALYZER_H__
 
#include "alganalyzer.h"
#include "openssl/des.h"
#include "utilities.h"

struct DESContext {
    DES_cblock Key;
    DES_key_schedule Subkeys;

    MemRegion KeyRegion;
    MemRegion SubkeysRegion;
};

enum DESCryptType {
    DESCRYPT_ENCRYPT,
    DESCRYPT_DECRYPT,
};

struct DESCrypt {
    Array<byte> Input, Output;
    MemRegion InputRegion, OutputRegion;
    TaintRegion MsgRegion;
    uint ContextIndex;
    DESCryptType Type;
    int BeginSeq, EndSeq;
    bool Ignored;   // used to indicate temporary result

    DESCrypt(cpbyte input, cpbyte output, const MemRegion &rin, 
        const MemRegion &rout, const TaintRegion &tr,
        uint idx, DESCryptType t, int begSeq, int endSeq);
};

class DESAnalyzer : public AlgorithmAnalyzer {
public:
    virtual ~DESAnalyzer();
    virtual bool OnOriginalProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual bool OnInputProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;
    virtual void OnComplete() override;

public:
    static const uint KeySize = sizeof(DES_cblock);
    static const uint BlockSize = sizeof(DES_cblock);
    static const uint SubKeySize = sizeof(DES_key_schedule);
private:
    void TestKeySchedule(const ProcContext &ctx, const MemRegion &input, const MemRegion &output);
    bool TestCrypt(const ProcContext &ctx, const MemRegion &input, 
        const MemRegion &output, const TaintRegion &tr);
    bool TestCryptMode(const ProcContext &ctx, const MemRegion &input, const MemRegion &output, const TaintRegion &tr);
    bool TestCryptModeCBC(DESContext &des, const ProcContext &ctx, const MemRegion &input, const MemRegion &output, const TaintRegion &tr);
    bool OnFoundCrypt(const ProcContext &ctx, cpbyte input, cpbyte output, const MemRegion &rin, 
        const MemRegion &rout, const TaintRegion &tr, uint ctxIndex, DESCryptType type);
    void ClearCrypts();
private:
    std::vector<DESContext> m_contexts;
    std::vector<DESCrypt *> m_crypts;
};
 
#endif // __PROPHET_PROTOCOL_ALGORITHMS_DES_ANALYZER_H__