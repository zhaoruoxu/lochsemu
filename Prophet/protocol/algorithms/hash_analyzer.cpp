#include "stdafx.h"
#include "hash_analyzer.h"
#include <openssl/md5.h>

void MD5Analyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
//     for (auto &output : ctx.OutputRegions) {
//         if (output.Len != 16) continue;
//         byte md[16];
//         FillMemRegionBytes(ctx.Outputs, output, md);
//         if (md[0] == 0x83 && md[1] == 0x3e && md[2] == 0x05) {
//             ctx.Dump(StdOut(), false);
//         }
//     }
//     
//     
//     if ((ctx.Proc->Entry() & 0xfff) == 0xe90) {
//         LxInfo("msg %08x - %d\n", m_algEngine->GetMessage()->GetRegion().Addr,
//             m_algEngine->GetMessage()->GetRegion().Len);
//         ctx.Dump(StdOut(), false);
//     }
//     
    for (auto &input : ctx.InputRegions) {
        //if (input.Len % 16 != 0) continue;
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        for (auto &output : ctx.OutputRegions) {
            if (output.Len != MDSize) continue;
            TestMD5(ctx, input, output, trs[0]);
        }
    }
}

void MD5Analyzer::OnInputProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    //ctx.Dump(StdOut(), true);


}

void MD5Analyzer::OnProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{

}

void MD5Analyzer::TestMD5(const ProcContext &ctx, const MemRegion &input, 
                          const MemRegion &output, const TaintRegion &tr )
{
    pbyte in = new byte[input.Len];
    byte md[MDSize];
    FillMemRegionBytes(ctx.Inputs, input, in);
    FillMemRegionBytes(ctx.Outputs, output, md);

//     int len = -1;
//     for (int i = input.Len - 1 - 8; i >= 0; i--) {
//         // Search from last padding byte, ignore 8-byte length at the end
//         if (in[i] == 0x80) {
//             len = i; break;
//         } else if (in[i] != 0) {
//             break;
//         }
//     }
//     if (len == -1) {
//         SAFE_DELETE_ARRAY(in); return;
//     }
// 
//     in[len] = 0;
//     Message *parent = m_algEngine->GetMessage();
//     const char * r = strstr((const char *) parent->GetRaw(), (const char *) in);
//     if (r == NULL) {
//         SAFE_DELETE_ARRAY(in); return;
//     }

    byte actualMd[MDSize];
    MD5((const byte *) in, input.Len, actualMd);
    if (CompareByteArray(md, actualMd, MDSize) == 0) {
        AlgTag *tag = new AlgTag("MD5", "Message Digest");
        tag->Params.push_back(new AlgParam("Message", input, in));
        tag->Params.push_back(new AlgParam("Digest", output, md));
        //MemRegion rin(parent->GetRegion().Addr + ((cpbyte) r - parent->GetRaw()), len);
        Message *parent = m_algEngine->GetMessage();
        Message *newMsg = new Message(output, md, parent, 
            parent->GetRegion().SubRegion(tr), tag, false);
        LxInfo("MD5 Message Digest: %08x-%08x\n", output.Addr, output.Addr + output.Len - 1);
        m_algEngine->GetMessageManager()->EnqueueMessage(newMsg,
            ctx.Level == 0 ? ctx.EndSeq+1:ctx.BeginSeq, parent->GetTraceEnd());
    }

    SAFE_DELETE_ARRAY(in);
}
