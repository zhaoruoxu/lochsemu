#include "stdafx.h"
#include "des_analyzer.h"


void DESAnalyzer::OnProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    
}

void DESAnalyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    //ctx.Dump(StdOut(), true);

    for (auto &input : ctx.InputRegions) {
        if (input.Len != BlockSize) continue;
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;
        for (auto &output : ctx.OutputRegions) {
            if (output.Len != BlockSize) continue;
            Taint tout = GetMemRegionTaintAnd(ctx.Outputs, output);
            if ((tout & tin) != tin) continue;
            TestCrypt(ctx, input, output, trs[0]);
        }
    }
}

void DESAnalyzer::OnInputProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    //ctx.Dump(StdOut(), true);

    for (auto &input : ctx.InputRegions) {
        if (input.Len != KeySize) continue;
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        for (auto &output : ctx.OutputRegions) {
            if (output.Len != SubKeySize) continue;
            Taint tout = GetMemRegionTaintAnd(ctx.Outputs, output);
            if ((tout & tin) != tin) continue;
            // Taint check okay
            TestKeySchedule(ctx, input, output);
        }
    }
}

void DESAnalyzer::TestKeySchedule( const ProcContext &ctx, const MemRegion &input, const MemRegion &output )
{
    for (auto &c : m_contexts) {
        if (c.KeyRegion == input && c.SubkeysRegion == output) 
            return;
    }
    DESContext des;
    DES_key_schedule schedule;
    FillMemRegionBytes(ctx.Inputs, input, (pbyte) &des.Key);
    FillMemRegionBytes(ctx.Outputs, output, (pbyte) &des.Subkeys);
    DES_set_key_checked(&des.Key, &schedule);
    if (CompareByteArray((pbyte) &schedule, (pbyte) &des.Subkeys, SubKeySize) != 0)
        return;

    des.KeyRegion = input;
    des.SubkeysRegion = output;
    m_contexts.push_back(des);
    LxInfo("Found DES Key schedule\n");
}

void DESAnalyzer::TestCrypt(const ProcContext &ctx, const MemRegion &input, 
                            const MemRegion &output, const TaintRegion &tr )
{
    for (uint i = 0; i < m_contexts.size(); i++) {
        DESContext &des = m_contexts[i];
        DES_cblock bin, bout;
        FillMemRegionBytes(ctx.Inputs, input, (pbyte) &bin);
        FillMemRegionBytes(ctx.Outputs, output, (pbyte) &bout);
        DES_cblock bactual;
        DES_ecb_encrypt(&bin, &bactual, &des.Subkeys, DES_DECRYPT);
        if (CompareByteArray((pbyte) &bout, (pbyte) &bactual, BlockSize) == 0) {
            LxInfo("Found DES decrypt\n");
            OnFoundCrypt(ctx, (cpbyte) &bin, (cpbyte) &bout, input, output, 
                tr, i, DESCRYPT_DECRYPT);
        }
    }
}

void DESAnalyzer::OnFoundCrypt(const ProcContext &ctx, cpbyte input, cpbyte output, 
                               const MemRegion &rin, const MemRegion &rout, 
                               const TaintRegion &tr, uint ctxIndex, DESCryptType type )
{
    for (auto &crypt : m_crypts) {
        if (crypt->ContextIndex == ctxIndex && crypt->Type == type &&
            crypt->InputRegion.CanMerge(rin) && crypt->OutputRegion.CanMerge(rout))
        {
            if (!crypt->InputRegion.TryMerge(rin) || !crypt->OutputRegion.TryMerge(rout))
                LxFatal("merging failed\n");
            if (!crypt->MsgRegion.TryMerge(tr))
                LxFatal("taint merge failed\n");
            crypt->Input.Append(input, rin.Len);
            crypt->Output.Append(output, rout.Len);
            return;
        }
    }

    DESCrypt *c = new DESCrypt(input, output, rin, rout, tr, 
        ctxIndex, type, ctx.EndSeq + 1, m_algEngine->GetMessage()->GetTraceEnd());
    m_crypts.push_back(c);
}

void DESAnalyzer::OnComplete()
{
    for (auto &crypt : m_crypts) {
        LxInfo("DES crypt: len = %d\n", crypt->InputRegion.Len);

        std::string desc = "Block Cipher";
        if (crypt->Type == DESCRYPT_DECRYPT)
            desc += "/Decryption";
        else if (crypt->Type == DESCRYPT_ENCRYPT)
            desc += "/Encryption";
        const DESContext &ctx = m_contexts[crypt->ContextIndex];
        AlgTag *tag = new AlgTag("DES", desc.c_str());
        tag->Params.push_back(new AlgParam("Key", ctx.KeyRegion, (cpbyte) &ctx.Key));
        tag->Params.push_back(new AlgParam("Input", crypt->InputRegion, crypt->Input.Get()));
        tag->Params.push_back(new AlgParam("Output", crypt->OutputRegion, crypt->Output.Get()));

        Message *parent = m_algEngine->GetMessage();
        Message *msg = new Message(crypt->OutputRegion, 
            crypt->Output.Get(), parent, 
            parent->GetRegion().SubRegion(crypt->MsgRegion), tag, true);
        LxInfo("DES sub-message: [%08x-%08x]\n", msg->GetRegion().Addr,
            msg->GetRegion().Addr + msg->GetRegion().Len - 1);
        m_algEngine->GetMessageManager()->EnqueueMessage(msg, crypt->BeginSeq, crypt->EndSeq);
    }
}

DESAnalyzer::~DESAnalyzer()
{
    ClearCrypts();
}

void DESAnalyzer::ClearCrypts()
{
    for (auto &c : m_crypts) {
        SAFE_DELETE(c);
    }
    m_crypts.clear();
}

DESCrypt::DESCrypt(cpbyte input, cpbyte output, const MemRegion &rin, 
                   const MemRegion &rout, const TaintRegion &tr, uint idx, 
                   DESCryptType t, int begSeq, int endSeq )
{
    InputRegion = rin;
    OutputRegion = rout;
    MsgRegion = tr;
    Input.Append(input, InputRegion.Len);
    Output.Append(output, OutputRegion.Len);
    ContextIndex = idx;
    Type = t;
    BeginSeq = begSeq;
    EndSeq = endSeq;
}
