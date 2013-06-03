#include "stdafx.h"
#include "hash_analyzer.h"
#include <openssl/md5.h>

void MD5Analyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    
}

void MD5Analyzer::OnInputProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    ctx.Dump(StdOut(), true);

    for (auto &input : ctx.InputRegions) {
        if (input.Len % 16 != 0) continue;
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        for (auto &output : ctx.OutputRegions) {
            if (output.Len != MDSize) continue;
            TestMD5(ctx, input, output);
        }
    }
}

void MD5Analyzer::OnProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    
}

void MD5Analyzer::TestMD5( const ProcContext &ctx, const MemRegion &input, const MemRegion &output )
{
    pbyte in = new byte[input.Len];
    byte md[MDSize];
    FillMemRegionBytes(ctx.Inputs, input, in);
    FillMemRegionBytes(ctx.Outputs, output, md);

    int len = -1;
    for (int i = input.Len - 1 - 8; i >= 0; i--) {
        // Search from last padding byte, ignore 8-byte length at the end
        if (in[i] == 0x80) {
            len = i; break;
        } else if (in[i] != 0) {
            break;
        }
    }
    if (len == -1) {
        SAFE_DELETE_ARRAY(in); return;
    }

    in[len] = 0;
    Message *parent = m_algEngine->GetMessage();
    const char * r = strstr((const char *) parent->GetRaw(), (const char *) in);
    if (r == NULL) {
        SAFE_DELETE_ARRAY(in); return;
    }

    byte actualMd[MDSize];
    MD5((const byte *) r, len, actualMd);
    if (CompareByteArray(md, actualMd, MDSize) == 0) {
        AlgTag *tag = new AlgTag("MD5", "Message Digest");
        tag->Params.push_back(new AlgParam("Message", MemRegion(input.Addr, len), in));
        tag->Params.push_back(new AlgParam("Digest", output, md));
        MemRegion rin(parent->GetRegion().Addr + ((cpbyte) r - parent->GetRaw()), len);
        Message *newMsg = new Message(output, md, parent, rin, tag, false);
        LxInfo("MD5 Message Digest: %08x-%08x\n", rin.Addr, rin.Addr + rin.Len - 1);
        m_algEngine->GetMessageManager()->EnqueueMessage(newMsg,
            ctx.Level == 0 ? ctx.EndSeq+1:ctx.BeginSeq, parent->GetTraceEnd());
    }

    SAFE_DELETE_ARRAY(in);
}
