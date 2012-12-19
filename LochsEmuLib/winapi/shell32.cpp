#include "stdafx.h"
#include "winapi.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Shell32_SHGetMalloc(Processor *cpu)
{
    RET_VALUE = (u32) SHGetMalloc(
        (LPMALLOC *)    PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Shell32_SHGetPathFromIDListA(Processor *cpu)
{
    PCIDLIST_ABSOLUTE pcid = (PCIDLIST_ABSOLUTE) PARAM(0);
    RET_VALUE = (u32) SHGetPathFromIDListA(
        //(PCIDLIST_ABSOLUTE) PARAM(0),
        pcid, 
        (LPSTR)             PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Shell32_SHGetSpecialFolderLocation(Processor *cpu)
{
    RET_VALUE = (u32) SHGetSpecialFolderLocation(
        (HWND)      PARAM(0),
        (int)       PARAM(1),
        (PIDLIST_ABSOLUTE *)  PARAM_PTR(2)
        );
    RET_PARAMS(3);
}

END_NAMESPACE_LOCHSEMU()