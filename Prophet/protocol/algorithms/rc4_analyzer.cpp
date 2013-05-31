#include "stdafx.h"
#include "rc4_analyzer.h"


void RC4Analyzer::OnOriginalProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{

}

void RC4Analyzer::OnInputProcedure( ExecuteTraceEvent &event, const ProcContext &ctx )
{
    ctx.Dump(StdOut(), true);
}
