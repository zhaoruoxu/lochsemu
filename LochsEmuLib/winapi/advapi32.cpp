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

uint Advapi32_RegCloseKey(Processor *cpu)
{
    RET_VALUE = (u32) RegCloseKey(
        (HKEY)      PARAM(0)
        );
    RET_PARAMS(1);
}

uint Advapi32_RegEnumKeyExA(Processor *cpu)
{
    RET_VALUE = (u32) RegEnumKeyExA(
        (HKEY)      PARAM(0),
        (DWORD)     PARAM(1),
        (LPSTR)     PARAM_PTR(2),
        (LPDWORD)   PARAM_PTR(3),
        (LPDWORD)   PARAM_PTR(4),
        (LPSTR)     PARAM_PTR(5),
        (LPDWORD)   PARAM_PTR(6),
        (PFILETIME) PARAM_PTR(7)
        );
    RET_PARAMS(8);
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

uint Advapi32_RegQueryInfoKeyA(Processor *cpu)
{
    RET_VALUE = (u32) RegQueryInfoKeyA(
        (HKEY)      PARAM(0),
        (LPSTR)     PARAM_PTR(1),
        (LPDWORD)   PARAM_PTR(2),
        (LPDWORD)   PARAM_PTR(3),
        (LPDWORD)   PARAM_PTR(4),
        (LPDWORD)   PARAM_PTR(5),
        (LPDWORD)   PARAM_PTR(6),
        (LPDWORD)   PARAM_PTR(7),
        (LPDWORD)   PARAM_PTR(8),
        (LPDWORD)   PARAM_PTR(9),
        (LPDWORD)   PARAM_PTR(10),
        (PFILETIME) PARAM_PTR(11)
        );
    RET_PARAMS(12);
}

uint Advapi32_RegQueryValueExA(Processor *cpu)
{
    RET_VALUE = (u32) RegQueryValueExA(
        (HKEY)      PARAM(0),
        (LPCSTR)    PARAM_PTR(1),
        (LPDWORD)   PARAM_PTR(2),
        (LPDWORD)   PARAM_PTR(3),
        (LPBYTE)    PARAM_PTR(4),
        (LPDWORD)   PARAM_PTR(5)
        );
    RET_PARAMS(6);
}

END_NAMESPACE_LOCHSEMU()