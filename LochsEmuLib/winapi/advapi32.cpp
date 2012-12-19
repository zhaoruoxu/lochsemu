#include "stdafx.h"
#include "winapi.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Advapi32_CryptAcquireContextW(Processor *cpu)
{
    RET_VALUE = (u32) CryptAcquireContextW(
        (HCRYPTPROV *)  PARAM_PTR(0),
        (LPCWSTR)       PARAM_PTR(1),
        (LPCWSTR)       PARAM_PTR(2),
        (DWORD)         PARAM(3),
        (DWORD)         PARAM(4)
        );
    RET_PARAMS(5);

}

uint Advapi32_CryptGenRandom(Processor *cpu)
{
    RET_VALUE = (u32) CryptGenRandom(
        (HCRYPTPROV)    PARAM(0),
        (DWORD)         PARAM(1),
        (BYTE *)        PARAM_PTR(2)
        );
    RET_PARAMS(3);
}

uint Advapi32_CryptReleaseContext(Processor *cpu)
{
    RET_VALUE = (u32) CryptReleaseContext(
        (HCRYPTPROV)    PARAM(0),
        (ULONG_PTR)     PARAM(1)
        );
    RET_PARAMS(2);
}

uint Advapi32_RegOpenKeyExA(Processor *cpu)
{
    RET_VALUE = (u32) RegOpenKeyExA(
        (HKEY)      PARAM(0),
        (LPCSTR)    PARAM_PTR(1),
        (DWORD)     PARAM(2),
        (REGSAM)    PARAM(3),
        (PHKEY)     PARAM_PTR(4)
        );
    RET_PARAMS(5);
}

uint Advapi32_RegOpenKeyExW(Processor *cpu)
{
    RET_VALUE = (u32) RegOpenKeyExW(
        (HKEY)      PARAM(0),
        (LPCWSTR)   PARAM_PTR(1),
        (DWORD)     PARAM(2),
        (REGSAM)    PARAM(3),
        (PHKEY)     PARAM_PTR(4)
        );
    RET_PARAMS(5);
}

END_NAMESPACE_LOCHSEMU()