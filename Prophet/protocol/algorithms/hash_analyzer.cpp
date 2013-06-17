#include "stdafx.h"
#include "hash_analyzer.h"
#include <openssl/md5.h>

bool MD5Analyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    for (auto &input : ctx.InputRegions) {
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        for (auto &output : ctx.OutputRegions) {
            if (output.Len != MDSize) continue;
            if (TestMD5(ctx, input, output, trs[0]))
                return true;
        }
    }
    return false;
}

bool MD5Analyzer::TestMD5(const ProcContext &ctx, const MemRegion &input, 
                          const MemRegion &output, const TaintRegion &tr )
{
    pbyte in = new byte[input.Len];
    byte md[MDSize];
    FillMemRegionBytes(ctx.Inputs, input, in);
    FillMemRegionBytes(ctx.Outputs, output, md);

    byte actualMd[MDSize];
    MD5((const byte *) in, input.Len, actualMd);
    bool found = false;
    if (CompareByteArray(md, actualMd, MDSize) == 0) {
        AlgTag *tag = new AlgTag("MD5", "Message Digest");
        tag->Params.push_back(new AlgParam("Message", input, in));
        tag->Params.push_back(new AlgParam("Digest", output, md));
        Message *parent = m_algEngine->GetMessage();
        Message *newMsg = new Message(output, md, parent, 
            parent->GetRegion().SubRegion(tr), tag, false);
        LxInfo("MD5 Message Digest: %08x-%08x\n", output.Addr, output.Addr + output.Len - 1);
        m_algEngine->GetMessageManager()->EnqueueMessage(newMsg,
            ctx.Level == 0 ? ctx.EndSeq+1:ctx.BeginSeq, parent->GetTraceEnd());\
        found = true;
    }

    SAFE_DELETE_ARRAY(in);
    return found;
}
