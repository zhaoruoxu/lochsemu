#include "stdafx.h"
#include "des_analyzer.h"

static void DES_cblock_xor(const DES_cblock &a, const DES_cblock &b, DES_cblock &r)
{
    for (int i = 0; i < sizeof(DES_cblock); i++)
        r[i] = a[i] ^ b[i];
}

bool DESAnalyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
#if 0
    ctx.Dump(StdOut(), true);
#endif

    if (ctx.Level > 1) return false;
    for (auto &input : ctx.InputRegions) {
        if (input.Len % BlockSize != 0) continue;
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;
        bool hasSuccess = false;
        for (auto &output : ctx.OutputRegions) {
            if (output.Len % BlockSize != 0) continue;

            if (output.Len == input.Len + BlockSize) {
                // possible CFB, hahahaho
                for (auto &des : m_contexts) {
                    if (TestCryptModeCFB(des, ctx, input, output, trs[0])) {
                        hasSuccess = true; break;
                    }
                }
            } else if (output.Len > BlockSize && output.Len == input.Len) {
                if (TestCryptMode(ctx, input, output, trs[0])) {
                    hasSuccess = true; break;
                }
            } else if (output.Len == BlockSize) {
                Taint tout = GetMemRegionTaintAnd(ctx.Outputs, output);
                if ((tout & tin) != tin) continue;
                if (TestCrypt(ctx, input, output, trs[0])) {
                    hasSuccess = true;
                }
            }
                
        }
        if (hasSuccess) return true;
    }
    return false;
}

bool DESAnalyzer::OnInputProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
#if 0
    ctx.Dump(StdOut(), true);
#endif

    if (ctx.Level > 1) return false;
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
    return false;
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

bool DESAnalyzer::TestCrypt(const ProcContext &ctx, const MemRegion &input, 
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
            return OnFoundCrypt(ctx, (cpbyte) &bin, (cpbyte) &bout, input, output, 
                tr, i, DESCRYPT_DECRYPT);
        }
    }
    return false;
}

bool DESAnalyzer::TestCryptMode( const ProcContext &ctx, const MemRegion &input, const MemRegion &output, const TaintRegion &tr )
{
    // first check block taint
    Assert(input.Len == output.Len && (input.Len % BlockSize) == 0);
    for (int block = 0; block < (int) (input.Len / BlockSize); block++) {
        MemRegion bin(input.Addr + block * BlockSize, BlockSize);
        MemRegion bout(output.Addr + block * BlockSize, BlockSize);
        Taint tbin = GetMemRegionTaintOr(ctx.Inputs, bin);
        Taint tbout = GetMemRegionTaintAnd(ctx.Outputs, bout);
        if ((tbin & tbout) != tbin) return false;
    }

    for (uint i = 0; i < m_contexts.size(); i++) {
        DESContext &des = m_contexts[i];
        // retrieve IV from first block
        if (TestCryptModeCBC(des, ctx, input, output, tr))
            return true;
        //if (TestCryptModeCFB(des, ctx, input, output, tr))
        //    return true;
    }

    return false;
}

bool DESAnalyzer::TestCryptModeCBC(DESContext &des, const ProcContext &ctx, const MemRegion &input, const MemRegion &output, const TaintRegion &tr )
{
    pbyte pin = new byte[input.Len];
    pbyte pout = new byte[output.Len];
    FillMemRegionBytes(ctx.Inputs, input, pin);
    FillMemRegionBytes(ctx.Outputs, output, pout);
    bool result = false;

    DES_cblock iv, dec;
    DES_ecb_encrypt((const_DES_cblock *) pin, &dec, &des.Subkeys, DES_DECRYPT);
    DES_cblock_xor((const DES_cblock &) *pout, dec, iv);

    pbyte pdec = new byte[output.Len];

    DES_cbc_encrypt(pin, pdec, input.Len, &des.Subkeys, &iv, DES_DECRYPT);

    if (CompareByteArray(pdec, pout, output.Len) == 0) {
        LxInfo("Found DES-CBC decryption\n");
        AlgTag *tag = new AlgTag("DES-CBC", "Decryption");
        tag->Params.push_back(new AlgParam("Key", des.KeyRegion, (cpbyte) &des.Key));
        tag->Params.push_back(new AlgParam("IV", MemRegion(-1, BlockSize), (cpbyte) &iv));
        tag->Params.push_back(new AlgParam("Ciphertext", output, pout));
        tag->Params.push_back(new AlgParam("Plaintext", input, pin));

        m_algEngine->EnqueueNewMessage(output, pout, tr, tag, ctx, true);
        result = true;
    }

    SAFE_DELETE_ARRAY(pdec);
    SAFE_DELETE_ARRAY(pin);
    SAFE_DELETE_ARRAY(pout);
    // retrieve IV from first block
    return result;
}

bool DESAnalyzer::TestCryptModeCFB( DESContext &des, const ProcContext &ctx, const MemRegion &input, const MemRegion &output, const TaintRegion &tr )
{
    pbyte pin = new byte[input.Len];
    pbyte pdec = new byte[input.Len];
    pbyte pout = new byte[output.Len];
    FillMemRegionBytes(ctx.Inputs, input, pin);
    FillMemRegionBytes(ctx.Outputs, output, pout);
    ZeroMemory(pdec, input.Len);

    bool result = false;
    for (auto &iv : ctx.InputRegions) {
        if (iv.Len != BlockSize) continue;
        DES_cblock ivBlock;
        FillMemRegionBytes(ctx.Inputs, iv, (pbyte) &ivBlock);

        DES_cfb_encrypt(pin, pdec, 1, input.Len, &des.Subkeys, &ivBlock, DES_DECRYPT);
        for (uint offset = 0; offset <= output.Len - input.Len; offset++) {
            if (CompareByteArray(pout + offset, pdec, input.Len) == 0) {
                LxInfo("Found DES-CFB decryption\n");

                FillMemRegionBytes(ctx.Inputs, iv, (pbyte) &ivBlock);
                AlgTag *tag = new AlgTag("DES-CFB", "Decryption");
                tag->Params.push_back(new AlgParam("Key", des.KeyRegion, (cpbyte) &des.Key));
                tag->Params.push_back(new AlgParam("IV", iv, (cpbyte) &ivBlock));
                tag->Params.push_back(new AlgParam("Ciphertext", output, pout));
                tag->Params.push_back(new AlgParam("Plaintext", input, pin));

                m_algEngine->EnqueueNewMessage(output, pout, tr, tag, ctx, true);
                result = true;
                goto L_END;
            }
        }
    }

    L_END:
    SAFE_DELETE_ARRAY(pin);
    SAFE_DELETE_ARRAY(pout);
    SAFE_DELETE_ARRAY(pdec);
    return result;
}

bool DESAnalyzer::OnFoundCrypt(const ProcContext &ctx, cpbyte input, cpbyte output, 
                               const MemRegion &rin, const MemRegion &rout, 
                               const TaintRegion &tr, uint ctxIndex, DESCryptType type )
{
    for (auto &crypt : m_crypts) {
        if (crypt->ContextIndex == ctxIndex && crypt->Type == type &&
            crypt->InputRegion.CanMerge(rin))
        {
            if (crypt->OutputRegion.CanMerge(rout)) {
                if (!crypt->InputRegion.TryMerge(rin) || !crypt->OutputRegion.TryMerge(rout))
                    LxFatal("merging failed\n");
                if (!crypt->MsgRegion.TryMerge(tr))
                    LxFatal("taint merge failed\n");
                crypt->Input.Append(input, rin.Len);
                crypt->Output.Append(output, rout.Len);
                crypt->BeginSeq = max(crypt->BeginSeq, ctx.EndSeq + 1);
                return true;
            } else if (crypt->OutputRegion == rout) {
                if (!crypt->InputRegion.TryMerge(rin))
                    LxFatal("merging failed\n");
                crypt->Ignored = true;
                return false;   // Ignore this when using the same output buffer
            }
        }
    }

    DESCrypt *c = new DESCrypt(input, output, rin, rout, tr, 
        ctxIndex, type, ctx.EndSeq + 1, m_algEngine->GetMessage()->GetTraceEnd());
    m_crypts.push_back(c);
    return true;
}

void DESAnalyzer::OnComplete()
{
    for (auto &crypt : m_crypts) {
        if (crypt->Ignored) continue;
        LxInfo("DES crypt: len = %d\n", crypt->InputRegion.Len);

        std::string desc = "Block Cipher";
        if (crypt->Type == DESCRYPT_DECRYPT)
            desc += "Decryption";
        else if (crypt->Type == DESCRYPT_ENCRYPT)
            desc += "Encryption";
        const DESContext &ctx = m_contexts[crypt->ContextIndex];
        AlgTag *tag = new AlgTag("DES-ECB", desc.c_str());
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
    Ignored = false;
}
