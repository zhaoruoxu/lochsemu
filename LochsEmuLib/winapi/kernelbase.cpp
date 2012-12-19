#include "stdafx.h"
#include "winapi.h"
#include "processor.h"
#include "emulator.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Kernelbase_GetLocaleInfoA(Processor *cpu)
{
    RET_VALUE = (u32) GetLocaleInfoA(
        (LCID)      PARAM(0),
        (LCTYPE)    PARAM(1),
        (LPSTR)     PARAM_PTR(2),
        (int)       PARAM(3)
        );
    RET_PARAMS(4);
}


END_NAMESPACE_LOCHSEMU()