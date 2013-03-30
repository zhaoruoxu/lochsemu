#include "stdafx.h"
#include "winapi.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()


uint Netapi32_NetApiBufferFree(Processor *cpu)
{
//     RET_VALUE = (u32) NetApiBufferFree(
//         (LPVOID)        PARAM_PTR(0)
//         );
    LxWarning("!! TODO : NetApiBufferFree() !!\n");
    RET_PARAMS(1);
}

uint Netapi32_NetStatisticsGet(Processor *cpu)
{
    DWORD level = (DWORD)   PARAM(2);
    LPBYTE *    bufptr = (LPBYTE *) PARAM_PTR(4);

    RET_VALUE = (u32) NetStatisticsGet(
        (LPWSTR)        PARAM_PTR(0),
        (LPWSTR)        PARAM_PTR(1),
        level,
        (DWORD)         PARAM(3),
        bufptr
        );

    SyncObjectLock lock(*cpu->Mem);

    static u32 Base = 0x900000;
    const u32 BufferSize = RoundUp(sizeof(STAT_WORKSTATION_0));

    Base = cpu->Mem->FindFreePages(Base, BufferSize);

    V( cpu->Mem->Alloc(SectionDesc("NetApi_Buffer", cpu->GetCurrentModule()),
        Base, BufferSize, PAGE_READWRITE) );
    LxInfo("Allocated memory for NetStatisticsGet() at %08x, size %08x\n",
        Base, BufferSize);

    memcpy(cpu->Mem->GetRawData(Base), *bufptr, sizeof(STAT_WORKSTATION_0));
    *bufptr = reinterpret_cast<LPBYTE>(Base);
    Base += BufferSize;

    RET_PARAMS(5);
}

END_NAMESPACE_LOCHSEMU()