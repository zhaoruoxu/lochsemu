#include "stdafx.h"
#include "winapi.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Comctl32_InitCommonControls(Processor *cpu)
{
    InitCommonControls();
    RET_PARAMS(0);
}


END_NAMESPACE_LOCHSEMU()