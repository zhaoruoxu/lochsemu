#include "stdafx.h"
#include "simd.h"

BEGIN_NAMESPACE_LOCHSEMU()

X86SIMD::X86SIMD()
{
    Reset();
}

X86SIMD::~X86SIMD()
{

}

void X86SIMD::Initialize( void )
{
    Reset();
}   

void X86SIMD::Reset( void )
{
    ZeroMemory(MM, sizeof(u64) * LX_SIMD_NUM_MMX_REGS);
    ZeroMemory(XMM, sizeof(u128) * LX_SIMD_NUM_SSE_REGS);
    __asm {
        mov     ebx, this
        stmxcsr [ebx].MXCSR
    }
}

END_NAMESPACE_LOCHSEMU()