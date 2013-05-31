#include "stdafx.h"
#include "rc4_analyzer.h"
#include "cryptohelp.h"

void RC4Analyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    ctx.Dump(StdOut(), false);

    if (m_contexts.empty()) return;
    for (auto &r : ctx.InputRegions) {
        TestRC4Crypt(event, ctx, r);
    }
}

void RC4Analyzer::OnInputProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    for (auto &region : ctx.OutputRegions) {
        TestKeySchedule(ctx, region);
    }
}

void RC4Analyzer::TestKeySchedule( const ProcContext &ctx, const MemRegion &region )
{
    if (region.Len < SboxLength) return;
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
            m_contexts.push_back(rc4ctx);
            LxInfo("RC4_KeySchedule: Key[%08x-%08x] Sbox[%08x-%08x]\n",
                key.Addr, key.Addr + key.Len - 1, sboxAddr, sboxAddr + 255);
        }
    }
    
}

void RC4Analyzer::TestRC4Crypt(ExecuteTraceEvent &event, 
                               const ProcContext &ctx, const MemRegion &region )
{
    // region必须为连续被taint的一块内存区域
    Taint tor;
    for (u32 o = 0; o < region.Len; o++) {
        tor |= ctx.Inputs.find(region.Addr + o)->second.Tnt;
    }
    auto tRegions = tor.GenerateRegions();
    if (tRegions.size() != 1) return;       // 不满足全部被连续taint
    
}

bool RC4Context::TryKeySchedule(const ProcContext &ctx, const MemRegion &key, 
                                const MemRegion &sbox )
{
    byte sboxData[RC4Analyzer::SboxLength];
    FillMemRegionBytes(ctx.Inputs, key, Key);
    FillMemRegionBytes(ctx.Outputs, sbox, sboxData);
    if (RC4_IsValidSbox(sboxData, Key, key.Len)) {
        KeyRegion = key; SboxRegion = sbox;
        return true;
    }
    return false;
}
