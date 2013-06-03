#include "stdafx.h"
#include "xor_analyzer.h"
#include "cryptohelp.h"

void ChainedXorAnalyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    for (auto &input : ctx.InputRegions) {
        if (input.Len <=4 ) continue;
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        for (auto &output : ctx.OutputRegions) {
            if (output.Len < input.Len) continue;

            if (input.Len == 29 || output.Len == 29) {
                ctx.Dump(StdOut(), true);
            }

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
                    TestCrypt(ctx, input, MemRegion(output.Addr + i - offset + 1, offset),
                        trs[0]);
                    break;
                }
            }
        }
    }
}

void ChainedXorAnalyzer::OnInputProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{


}

void ChainedXorAnalyzer::OnProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    //ctx.Dump(StdOut(), true);

}

void ChainedXorAnalyzer::TestCrypt( const ProcContext &ctx, const MemRegion &input, const MemRegion &output, const TaintRegion &tr )
{
    pbyte ct = new byte[input.Len];
    pbyte pt = new byte[output.Len];
    FillMemRegionBytes(ctx.Inputs, input, ct);
    FillMemRegionBytes(ctx.Outputs, output, pt);
    if (ChainedXor_IsValidDecrypt(ct, pt, input.Len)) {
        AlgTag *tag = new AlgTag("XOR", "Chained-XOR-Decrypt");
        tag->Params.push_back(new AlgParam("Ciphertext", input, ct));
        tag->Params.push_back(new AlgParam("Plaintext", output, pt));

        Message *parent = m_algEngine->GetMessage();
        Message *msg = new Message(output, pt, parent,
            parent->GetRegion().SubRegion(tr), tag, true);
        LxInfo("Chained-XOR sub-message: %08x-%08x\n",
            output.Addr, output.Addr + output.Len - 1);
        m_algEngine->GetMessageManager()->EnqueueMessage(msg, 
            ctx.Level == 0?ctx.EndSeq+1:ctx.BeginSeq, parent->GetTraceEnd());
    }
    SAFE_DELETE_ARRAY(ct);
    SAFE_DELETE_ARRAY(pt);
}
