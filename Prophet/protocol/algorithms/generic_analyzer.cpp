#include "stdafx.h"
#include "generic_analyzer.h"
#include "cryptohelp.h"

GenericSymmetricAnalyzer::~GenericSymmetricAnalyzer()
{
    for (auto &c : m_cryptos)
        SAFE_DELETE(c);
    m_cryptos.clear();
}

bool GenericSymmetricAnalyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
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
        bool hasSuccess = false;
        for (auto &output : ctx.OutputRegions) {
            Taint tout = GetMemRegionTaintAnd(ctx.Outputs, output);
            if ((tout & tin) != tin) continue;
            if (TestCrypt(ctx, input, output, trs[0])) {
                //ctx.Dump(StdOut(), true);
                hasSuccess = true;
            }
        }
        if (hasSuccess) return true;
    }
    return false;
}

bool GenericSymmetricAnalyzer::TestCrypt(const ProcContext &ctx, const MemRegion &input, 
                                const MemRegion &output, const TaintRegion &tr )
{
    if (input.Len != output.Len) return false;  // Symmetric!!
    pbyte pin = new byte[input.Len];
    pbyte pout = new byte[output.Len];

    FillMemRegionBytes(ctx.Inputs, input, pin);
    FillMemRegionBytes(ctx.Outputs, output, pout);

    bool result = false;
    for (auto &crypto : m_cryptos) {
        if (output == crypto->OutputRegion && crypto->InputRegion.CanMerge(input)) {
            if (!crypto->InputRegion.TryMerge(input))
                LxFatal("merging failed\n");
            crypto->Ignored = true;
            goto L_END;
        }   
        if (crypto->InputRegion.CanMerge(input) && crypto->OutputRegion.CanMerge(output))
        {
            if (!crypto->InputRegion.TryMerge(input) || !crypto->OutputRegion.TryMerge(output))
                LxFatal("Merge failed\n");
            if (!crypto->MsgRegion.TryMerge(tr))
                LxFatal("Taint merge failed\n");
            crypto->Input.Append(pin, input.Len);
            crypto->Output.Append(pout, output.Len);
            crypto->BeginSeq = max(crypto->BeginSeq, ctx.EndSeq + 1);
            result = true;
            goto L_END;
        }
    }

    GenericCrypto *g = new GenericCrypto(pin, pout, input, output, tr, 
        ctx.EndSeq + 1, m_algEngine->GetMessage()->GetTraceEnd(), ctx.Proc->Entry());
    m_cryptos.push_back(g);

L_END:
    SAFE_DELETE_ARRAY(pin);
    SAFE_DELETE_ARRAY(pout);
    return result;
}

void GenericSymmetricAnalyzer::OnComplete()
{
    for (auto &crypto : m_cryptos) {
        if (crypto->Ignored) continue;
        LxInfo("Generic crypto: len = %d\n", crypto->InputRegion.Len);
        AlgTag *tag = new AlgTag("Generic Crypto", "*", crypto->Proc);
        tag->AddParam("Input", crypto->InputRegion, crypto->Input.Get());
        tag->AddParam("Output", crypto->OutputRegion, crypto->Output.Get());

        Message *parent = m_algEngine->GetMessage();
        Message *msg = new Message(crypto->OutputRegion,
            crypto->Output.Get(), parent, parent->GetRegion().SubRegion(crypto->MsgRegion), tag, true);
        m_algEngine->GetMessageManager()->EnqueueMessage(msg, crypto->BeginSeq, crypto->EndSeq);
    }
}

GenericCrypto::GenericCrypto( cpbyte input, cpbyte output, const MemRegion &rin, 
                             const MemRegion &rout, const TaintRegion &tr, 
                             int begSeq, int endSeq, u32 proc )
{
    InputRegion = rin;
    OutputRegion = rout;
    MsgRegion = tr;
    Input.Append(input, InputRegion.Len);
    Output.Append(output, OutputRegion.Len);
    BeginSeq = begSeq;
    EndSeq = endSeq;
    Ignored = false;
    Proc = proc;
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

bool GenericEncodingAnalyzer::TestEncoding(const ProcContext &ctx, const MemRegion &input, 
                                           const MemRegion &output, const TaintRegion &tr,
                                           const Taint &tin)
{
    // check output's taint status
    //Taint tand = GetMemRegionTaintAnd(ctx.Outputs, output);
    //auto trsAnd = tand.GenerateRegions();

    Taint tor = GetMemRegionTaintOr(ctx.Outputs, output);
    if ((tor & tin) != tin) return false;
    int prevFirst, prevLast;
    //Taint t1 = ctx.Outputs.find(output.Addr)->second.Tnt;
    GetTaintRange(tor, &prevFirst, &prevLast);
    prevLast = prevFirst;   // start from first
    for (u32 addr = output.Addr + 1; addr < output.Addr + output.Len; addr++) {
        Taint t = ctx.Outputs.find(addr)->second.Tnt;
        int first, last;
        GetTaintRange(t, &first, &last);
        if (last < prevLast - 3)    // 3: failsafe
            return false;
        prevFirst = first; prevLast = last;
    }

    pbyte pin = new byte[input.Len];
    pbyte pout = new byte[output.Len];
    FillMemRegionBytes(ctx.Inputs, input, pin);
    FillMemRegionBytes(ctx.Outputs, output, pout);

    EntropyMetrics em(pin, input.Len, pout, output.Len);

    bool result = false;
    // check minimum difference rate
    int diffCount = 0;
    for (int i = 0; i < (int) min(input.Len, output.Len); i++) {
        if (pin[i] != pout[i]) diffCount++;
    }
    if ((double) diffCount / (double) min(input.Len, output.Len) < m_minDiffRate)
        goto L_END;

    LxInfo("Generic %s found: %08x-%08x\n", em.Result.c_str(), output.Addr,
        output.Len + output.Addr);
    AlgTag *tag = new AlgTag("Generic", em.Result, ctx.Proc->Entry());
    tag->AddParam("Input", input, pin);
    tag->AddParam("Output", output, pout);
    m_algEngine->EnqueueNewMessage(output, pout, tr, tag, ctx, false);
    result = true;

L_END:

    SAFE_DELETE_ARRAY(pin);
    SAFE_DELETE_ARRAY(pout);

    return result;
}

bool GenericChecksumAnalyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
#if 0
    ctx.Dump(StdOut(), true);
    LxInfo("eax = %08x\n", event.Context->EAX);
#endif
    
    for (auto &input : ctx.InputRegions) {
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        CheckRegisterChecksum(event, ctx, input, trs[0], tin);
    }

    return false;
}

bool GenericChecksumAnalyzer::CheckRegisterChecksum( 
    ExecuteTraceEvent &event, const ProcContext &ctx, const MemRegion &input, 
    const TaintRegion &tr, const Taint &tin )
{
    // check eax
    Taint teax = ctx.TRegs[LX_REG_EAX];
    if ((tin & teax) == tin) {
        pbyte pin = new byte[input.Len];
        FillMemRegionBytes(ctx.Inputs, input, pin);

        Checksums cs(pin, input.Len);
        std::string type;
        int len;
        cs.GetChecksumType(event.Context->AL, event.Context->EAX, type, len);

        LxInfo("Generic checksum register(%d): %08x-%08x\n", len, input.Addr, input.Addr + input.Len);

        AlgTag *tag = new AlgTag("Checksum", type, ctx.Proc->Entry());
        tag->AddParam("Data", input, pin);
        tag->AddParam("Checksum", MemRegion(0, len), (cpbyte) &event.Context->EAX);

        Message *parent = m_algEngine->GetMessage();
        Message *newMsg = new Message(MemRegion(0, len), (cpbyte) &event.Context->EAX,
            parent, parent->GetRegion().SubRegion(tr), tag, false);
        m_algEngine->GetMessageManager()->EnqueueMessage(newMsg, 0, 0);

        SAFE_DELETE_ARRAY(pin);
        return true;
    }
    return false;
}
