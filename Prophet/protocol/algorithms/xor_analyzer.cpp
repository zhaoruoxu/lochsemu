#include "stdafx.h"
#include "xor_analyzer.h"
#include "cryptohelp.h"

bool ChainedXorAnalyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    if (ctx.Level > 1) return false;

    for (auto &input : ctx.InputRegions) {
        if (input.Len <= 4) continue;
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        for (auto &output : ctx.OutputRegions) {
            if (output.Len < input.Len) continue;

            int offset = 0;
            for (int i = 0; i < (int) output.Len; i++) {
                Taint t = ctx.Outputs.find(output.Addr + i)->second.Tnt;
                Taint s1 = ctx.Inputs.find(input.Addr + offset)->second.Tnt;
                if (offset == 0 && t == s1) {
                    offset++;
                } else if (offset > 0) {
                    Taint s0 = ctx.Inputs.find(input.Addr + offset - 1)->second.Tnt;
                    if (t == (s0 | s1) && (t != s0) && (t != s1))
                        offset++;
                    else
                        offset = 0;
                }
                if (offset == input.Len) {
                    if (TestCrypt(ctx, input, MemRegion(output.Addr + i - offset + 1, offset), trs[0]))
                        return true;
                }
            }
        }
    }
    return false;
}


bool ChainedXorAnalyzer::TestCrypt( const ProcContext &ctx, const MemRegion &input, const MemRegion &output, const TaintRegion &tr )
{
    bool found = false;
    pbyte ct = new byte[input.Len];
    pbyte pt = new byte[output.Len];
    FillMemRegionBytes(ctx.Inputs, input, ct);
    FillMemRegionBytes(ctx.Outputs, output, pt);
    if (ChainedXor_IsValidDecrypt(ct, pt, input.Len)) {
        AlgTag *tag = new AlgTag("XOR", "Chained-XOR-Decrypt");
        Assert(ct[0] == pt[0]);
        tag->Params.push_back(new AlgParam("IV", MemRegion(input.Addr, 1), ct));
        tag->Params.push_back(new AlgParam("Ciphertext", input, ct));
        tag->Params.push_back(new AlgParam("Plaintext", output, pt));

        Message *parent = m_algEngine->GetMessage();
        Message *msg = new Message(output, pt, parent,
            parent->GetRegion().SubRegion(tr), tag, true);
        LxInfo("Chained-XOR sub-message: %08x-%08x\n",
            output.Addr, output.Addr + output.Len - 1);
        m_algEngine->GetMessageManager()->EnqueueMessage(msg, 
            ctx.Level == 0?ctx.EndSeq+1:ctx.BeginSeq, parent->GetTraceEnd());
        found = true;
    }
    SAFE_DELETE_ARRAY(ct);
    SAFE_DELETE_ARRAY(pt);
    return false;
}
