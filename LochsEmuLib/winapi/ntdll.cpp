#include "stdafx.h"
#include "winapi.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Ntdll_RtlGetNtVersionNumbers(Processor *cpu)
{
    OSVERSIONINFOA info;
    ZeroMemory(&info, sizeof(OSVERSIONINFOA));
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&info);
    LPDWORD pMajor = (LPDWORD) PARAM_PTR(0);
    if (pMajor) *pMajor = info.dwMajorVersion;
    LPDWORD pMinor = (LPDWORD) PARAM_PTR(1);
    if (pMinor) *pMinor = info.dwMinorVersion;
    LPDWORD pBuild = (LPDWORD) PARAM_PTR(2);
    if (pBuild) *pBuild = info.dwBuildNumber;
    RET_PARAMS(3);
}

uint Ntdll_RtlUnwind(Processor *cpu)
{
    NOT_IMPLEMENTED();
    RET_PARAMS(4);
}

END_NAMESPACE_LOCHSEMU()