#include "stdafx.h"
#include "win32.h"

BEGIN_NAMESPACE_LOCHSEMU()


LX_API PNT_TIB32 GetTIBPtr()
{
    PNT_TIB32 pTib;
    __asm mov eax, fs:[0x18]
    __asm mov pTib, eax
    return pTib;
}

LX_API WIN32_TEB * GetTEBPtr()
{
    return (WIN32_TEB *) GetTIBPtr();
}

LX_API WIN32_PEB * GetPEBPtr()
{
    WIN32_TEB *pTeb = GetTEBPtr();
    return (WIN32_PEB *) pTeb->PEB_Ptr;
}

END_NAMESPACE_LOCHSEMU()