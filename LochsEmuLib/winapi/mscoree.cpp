#include "stdafx.h"
#include "winapi.h"
#include "processor.h"


BEGIN_NAMESPACE_LOCHSEMU()


uint Mscoree_CorExitProcess(Processor *cpu)
{
//     CorExitProcess(
//         (int)   PARAM(0)
//         );
    cpu->Terminate(PARAM(0));
    RET_PARAMS(1);
}

END_NAMESPACE_LOCHSEMU()