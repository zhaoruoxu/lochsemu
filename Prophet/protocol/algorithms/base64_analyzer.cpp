#include "stdafx.h"
#include "base64_analyzer.h"



bool Base64Analyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    //ctx.Dump(StdOut(), false);

    for (auto &input : ctx.InputRegions) {
        Taint tin = GetMemRegionTaintOr(ctx.Inputs, input);
        if (!tin.IsAnyTainted()) continue;
        auto trs = tin.GenerateRegions();
        if (trs.size() != 1) continue;

        u32 olen = trs[0].Len / 4 * 3;
        for (auto &output : ctx.OutputRegions) {
            if (output.Len <= olen - 3 || output.Len > olen)
                continue;

            if (TestBase64(ctx, input, output, trs[0]))
                return true;
        }
    }

    return false;
}

bool Base64Analyzer::TestBase64(const ProcContext &ctx, const MemRegion &input, 
                                const MemRegion &output, const TaintRegion &tr )
{
    return false;
}
