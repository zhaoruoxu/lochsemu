#include "stdafx.h"
#include "winapi.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()


uint Netapi32_NetApiBufferFree(Processor *cpu)
{
    RET_VALUE = (u32) NetApiBufferFree(
        (LPVOID)        PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Netapi32_NetStatisticsGet(Processor *cpu)
{
    RET_VALUE = (u32) NetStatisticsGet(
        (LPWSTR)        PARAM_PTR(0),
        (LPWSTR)        PARAM_PTR(1),
        (DWORD)         PARAM(2),
        (DWORD)         PARAM(3),
        (LPBYTE *)      PARAM_PTR(4)
        );
    RET_PARAMS(5);
}

END_NAMESPACE_LOCHSEMU()