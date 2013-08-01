#include "stdafx.h"
#include "rc4_analyzer.h"
#include "cryptohelp.h"

bool RC4Analyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
#if 0
    ctx.Dump(StdOut(), false);
#endif

    if (ctx.Level > 1) return false;

    if (m_contexts.empty()) return false;
    for (auto &r : ctx.InputRegions) {
        if (TestRC4Crypt(ctx, r))
            return true;
    }
    return false;
}

bool RC4Analyzer::OnInputProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    if (ctx.Level > 1) return false;

    for (auto &region : ctx.OutputRegions) {
        TestKeySchedule(ctx, region);
    }
    return false;
}

void RC4Analyzer::TestKeySchedule( const ProcContext &ctx, const MemRegion &region )
{
    if (region.Len < SboxLength) return;
    if (region.Len > SboxLength) {
        return;     // WTF? Who wrote this
    }
    for (auto &c : m_contexts) {
        if (c.SboxRegion == region) return;
    }
    for (u32 offset = 0; offset <= region.Len - SboxLength; offset++) {
        TestKeySchedule(ctx, region.Addr + offset);
    }
}

void RC4Analyzer::TestKeySchedule( const ProcContext &ctx, u32 sboxAddr )
{
    auto &iter = ctx.Outputs.find(sboxAddr);
    Assert(iter != ctx.Outputs.end());
    Taint tor = iter->second.Tnt;
    Taint tand = tor;
    for (u32 i = 1; i < SboxLength; i++) {
        Taint t = ctx.Outputs.find(sboxAddr + i)->second.Tnt;
        tor |= t;
        tand &= t;
    }
    tor &= ~tand;       // now 'tor' should be tainted key

    auto keyRegions = tor.GenerateRegions();
    for (auto &tregion : keyRegions) {
        MemRegion key;
        if (!m_algEngine->GetTaint()->TryGetMemRegion(tregion, key))
            continue;
        RC4Context rc4ctx;
        if (rc4ctx.TryKeySchedule(ctx, key, MemRegion(sboxAddr, 256))) {
            bool exist = false;
            for (auto &c : m_contexts) {
                if (c.KeyRegion == rc4ctx.KeyRegion && c.SboxRegion == rc4ctx.SboxRegion)
                    exist = true;
            }
            if (!exist)
                m_contexts.push_back(rc4ctx);
            LxInfo("RC4_KeySchedule: Key[%08x-%08x] Sbox[%08x-%08x]\n",
                key.Addr, key.Addr + key.Len - 1, sboxAddr, sboxAddr + 255);
        }
    }
}

bool RC4Analyzer::TestRC4Crypt(const ProcContext &ctx, const MemRegion &region )
{
    Taint tor;
    for (u32 o = 0; o < region.Len; o++) {
        tor |= ctx.Inputs.find(region.Addr + o)->second.Tnt;
    }
    auto tRegions = tor.GenerateRegions();
    if (tRegions.size() != 1) return false;       
    
    for (auto &r : ctx.OutputRegions) {
        if (r.Len < region.Len) continue;
        u32 offset = 0;
        for (u32 i = 0; i < r.Len; i++) {
            if (ctx.Inputs.find(region.Addr + offset)->second.Tnt == 
                ctx.Outputs.find(r.Addr + i)->second.Tnt)
            {
                offset++;
            } else {
                offset = 0;
            }
            if (offset == region.Len) {
                // found match
                if (TestRC4Crypt(ctx, region, MemRegion(r.Addr + i - offset + 1, offset), tRegions[0]))
                    return true;
            }
        }
    }
    return false;
}

bool RC4Analyzer::TestRC4Crypt(const ProcContext &ctx, const MemRegion &input, 
                               const MemRegion &output, const TaintRegion &tin )
{
    Assert(input.Len == output.Len);
    bool found = false;
    for (auto &rc4ctx : m_contexts) {
        byte sbox[SboxLength];
        memcpy(sbox, rc4ctx.Sbox, SboxLength);
        int len = input.Len;
        pbyte pt = new byte[len];
        pbyte ct = new byte[len];
        FillMemRegionBytes(ctx.Inputs, input, pt);
        FillMemRegionBytes(ctx.Outputs, output, ct);
        if (RC4_IsValidCrypt(sbox, pt, ct, len)) {
            AlgTag *tag = new AlgTag("RC4", "RC4 stream cipher", ctx.Proc->Entry());
            tag->AddParam("Key", rc4ctx.KeyRegion, rc4ctx.Key);
            tag->AddParam("Input", input, pt);
            tag->AddParam("Output", output, ct);

            found = true;
            memcpy(rc4ctx.Sbox, sbox, SboxLength);
            LxInfo("RC4 sub-message: [%08x-%08x]\n", output.Addr, output.Addr + output.Len - 1);
            m_algEngine->EnqueueNewMessage(output, ct, tin, tag, ctx, true);
//             Message *parent = m_algEngine->GetMessage();
//             Message *submsg = new Message(output, ct, parent,
//                 parent->GetRegion().SubRegion(tin), tag, true);
//             m_algEngine->GetMessageManager()->EnqueueMessage(
//                 submsg, ctx.EndSeq+1, parent->GetTraceEnd());
        }
        SAFE_DELETE_ARRAY(pt);
        SAFE_DELETE_ARRAY(ct);
    }
    return found;
}

bool RC4Context::TryKeySchedule(const ProcContext &ctx, const MemRegion &key, 
                                const MemRegion &sbox )
{
    byte sboxData[SboxLength];
    FillMemRegionBytes(ctx.Inputs, key, Key);
    FillMemRegionBytes(ctx.Outputs, sbox, sboxData);
    if (RC4_IsValidSbox(sboxData, Key, key.Len)) {
        KeyRegion = key; SboxRegion = sbox;
        memcpy(Sbox, sboxData, SboxLength);
        return true;
    }
    return false;
}
