#include "stdafx.h"
#include "coprocessor.h"
#include "config.h"

BEGIN_NAMESPACE_LOCHSEMU()

Coprocessor::Coprocessor()
{
    Reset();
}

Coprocessor::~Coprocessor()
{

}

void Coprocessor::Reset()
{
    ZeroMemory(&m_context,     sizeof(FpuContext));
    __asm {
        finit
    }
    SaveContext();
}

LochsEmu::LxResult Coprocessor::Initialize( void )
{
    Assert(sizeof(FpuContext) == 108);
    m_preserveContext = LxConfig.GetInt("FPU", "PreserveContext", 1) != 0;
    Reset();
    RET_SUCCESS();
}

void Coprocessor::Wait( void )
{
    //LxWarning("TODO: FWAIT instruction\n");
}

void Coprocessor::RestoreContext( void )
{
    if (!m_preserveContext) return;
    __asm {
        finit
        mov     ebx, this
        frstor  [ebx].m_context
    }
}

void Coprocessor::SaveContext( void )
{
    if (!m_preserveContext) return;
    __asm {
        mov     ebx, this
        fsave   [ebx].m_context
    }
}

END_NAMESPACE_LOCHSEMU()