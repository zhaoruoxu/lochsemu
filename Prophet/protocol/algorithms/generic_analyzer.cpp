#include "stdafx.h"
#include "generic_analyzer.h"
#include "cryptohelp.h"

GenericCryptoAnalyzer::~GenericCryptoAnalyzer()
{
    for (auto &c : m_cryptos)
        SAFE_DELETE(c);
    m_cryptos.clear();
}

bool GenericCryptoAnalyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
#if 0
    ctx.Dump(StdOut(), true);
#endif

    if (ctx.Level > 1) return false;
    for (auto &input : ctx.InputRegions) {
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;
        if (trs[0].Len != input.Len) {
            LxWarning("weird\n"); continue;
        }
        for (auto &output : ctx.OutputRegions) {
            Taint tout = GetMemRegionTaintAnd(ctx.Outputs, output);
            if ((tout & tin) != tin) continue;
            if (TestCrypt(ctx, input, output, trs[0])) {
                ctx.Dump(StdOut(), true);
                return true;
            }
        }
    }
    return false;
}

bool GenericCryptoAnalyzer::TestCrypt(const ProcContext &ctx, const MemRegion &input, 
                                const MemRegion &output, const TaintRegion &tr )
{
    pbyte pin = new byte[input.Len];
    pbyte pout = new byte[output.Len];

    FillMemRegionBytes(ctx.Inputs, input, pin);
    FillMemRegionBytes(ctx.Outputs, output, pout);

    LxDebug("input entropy=%f, output entropy=%f\n",
        CalculateEntropy(pin, input.Len), CalculateEntropy(pout, output.Len));

    for (auto &crypto : m_cryptos) {
        if (input == crypto->InputRegion || output == crypto->OutputRegion)
            goto L_END;
        if (crypto->InputRegion.CanMerge(input) && crypto->OutputRegion.CanMerge(output))
        {
            if (!crypto->InputRegion.TryMerge(input) || !crypto->OutputRegion.TryMerge(output))
                LxFatal("Merge failed\n");
            if (!crypto->MsgRegion.TryMerge(tr))
                LxFatal("Taint merge failed\n");
            crypto->Input.Append(pin, input.Len);
            crypto->Output.Append(pout, output.Len);
            crypto->BeginSeq = max(crypto->BeginSeq, ctx.EndSeq + 1);
            goto L_END;
        }
    }

    GenericCrypto *g = new GenericCrypto(pin, pout, input, output, tr, 
        ctx.EndSeq + 1, m_algEngine->GetMessage()->GetTraceEnd());
    m_cryptos.push_back(g);

L_END:
    SAFE_DELETE_ARRAY(pin);
    SAFE_DELETE_ARRAY(pout);
    return true;
}

void GenericCryptoAnalyzer::OnComplete()
{
    for (auto &crypto : m_cryptos) {
        LxInfo("Generic crypto: len = %d\n", crypto->InputRegion.Len);
        AlgParam *input = new AlgParam("Input", crypto->InputRegion, crypto->Input.Get());
        AlgParam *output = new AlgParam("Output", crypto->OutputRegion, crypto->Output.Get());
        LxInfo("Hi=%f, Ho=%f\n", input->Entropy, output->Entropy);
        AlgTag *tag = new AlgTag("Generic", "**");
        tag->Params.push_back(input);
        tag->Params.push_back(output);

        Message *parent = m_algEngine->GetMessage();
        Message *msg = new Message(crypto->OutputRegion,
            crypto->Output.Get(), parent, parent->GetRegion().SubRegion(crypto->MsgRegion), tag, true);
        m_algEngine->GetMessageManager()->EnqueueMessage(msg, crypto->BeginSeq, crypto->EndSeq);
    }
}

GenericCrypto::GenericCrypto( cpbyte input, cpbyte output, const MemRegion &rin, const MemRegion &rout, const TaintRegion &tr, int begSeq, int endSeq )
{
    InputRegion = rin;
    OutputRegion = rout;
    MsgRegion = tr;
    Input.Append(input, InputRegion.Len);
    Output.Append(output, OutputRegion.Len);
    BeginSeq = begSeq;
    EndSeq = endSeq;
}

GenericEncodingAnalyzer::GenericEncodingAnalyzer( int minlen, double sd, double su, double dr )
    : m_minlen(minlen), m_maxSizeDown(sd), m_maxSizeUp(su), m_minDiffRate(dr)
{

}

bool GenericEncodingAnalyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
#if 0
    ctx.Dump(StdOut(), true);
#endif

    for (auto &input : ctx.InputRegions) {
        if ((int) input.Len < m_minlen) continue;
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        int olenmax = (int) (input.Len * m_maxSizeUp);
        int olenmin = (int) (input.Len * m_maxSizeDown);
        for (auto &output : ctx.OutputRegions) {
            if ((int) output.Len < olenmin || (int) output.Len > olenmax)
                continue;
            if (TestEncoding(ctx, input, output, trs[0], tin))
                return true;
        }
    }
    return false;
}

std::string GenericEncodingAnalyzer::EncodingTypeStr [] = {
    "Generic Encoding Decoding",
    "Generic Encoding",
    "Generic Decoding"
};

bool GenericEncodingAnalyzer::TestEncoding(const ProcContext &ctx, const MemRegion &input, 
                                           const MemRegion &output, const TaintRegion &tr,
                                           const Taint &tin)
{
    // check output's taint status
    //Taint tand = GetMemRegionTaintAnd(ctx.Outputs, output);
    //auto trsAnd = tand.GenerateRegions();

    Taint tor = GetMemRegionTaintOr(ctx.Outputs, output);
    if ((tor & tin) != tin) return false;

    pbyte pin = new byte[input.Len];
    pbyte pout = new byte[output.Len];
    FillMemRegionBytes(ctx.Inputs, input, pin);
    FillMemRegionBytes(ctx.Outputs, output, pout);

    bool result = false;

    // check minimum difference rate
    int diffCount = 0;
    for (int i = 0; i < (int) min(input.Len, output.Len); i++) {
        if (pin[i] != pout[i]) diffCount++;
    }
    if ((double) diffCount / (double) min(input.Len, output.Len) < m_minDiffRate)
        goto L_END;

    GenericEncodingType t = GenericEncodingOrDecoding;

    if (input.Len > output.Len)
        t = GenericDecoding;
    else if (input.Len < output.Len)
        t = GenericEncoding;

    LxInfo("%s found: %08x-%08x\n", EncodingTypeStr[t].c_str(), output.Addr,
        output.Len + output.Addr);
    AlgTag *tag = new AlgTag(EncodingTypeStr[t], "*");
    tag->Params.push_back(new AlgParam("Input", input, pin));
    tag->Params.push_back(new AlgParam("Output", output, pout));
    m_algEngine->EnqueueNewMessage(output, pout, tr, tag, ctx);
    result = true;

L_END:

    SAFE_DELETE_ARRAY(pin);
    SAFE_DELETE_ARRAY(pout);

    return result;
}




