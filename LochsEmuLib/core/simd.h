#pragma once

#ifndef __CORE_SIMD_H__
#define __CORE_SIMD_H__

#include "lochsemu.h"
#include "instruction.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define LX_SIMD_NUM_MMX_REGS    8
#define LX_SIMD_NUM_SSE_REGS    8

struct X86SIMD {

    u64         MM[LX_SIMD_NUM_MMX_REGS];
    u128        XMM[LX_SIMD_NUM_SSE_REGS];
    u32         MXCSR;

    X86SIMD();
    ~X86SIMD();

    void        Initialize(void);
    void        Reset(void);
    
    INLINE u64 &    MMReg(u32 regNum);
    INLINE u128 &   XMMReg(u32 regNum);
};


INLINE u64 & X86SIMD::MMReg( u32 regNum )
{
    switch (regNum) {
        case REG0: { return MM[0]; } break;
        case REG1: { return MM[1]; } break;
        case REG2: { return MM[2]; } break;
        case REG3: { return MM[3]; } break;
        case REG4: { return MM[4]; } break;
        case REG5: { return MM[5]; } break;
        case REG6: { return MM[6]; } break;
        case REG7: { return MM[7]; } break;
        default:
            LxFatal("Invalid MM register: %x\n", regNum);
    }
    Assert(0);
    return MM[0];
}

INLINE u128 & X86SIMD::XMMReg( u32 regNum )
{
    switch (regNum) {
        case REG0: { return XMM[0]; } break;
        case REG1: { return XMM[1]; } break;
        case REG2: { return XMM[2]; } break;
        case REG3: { return XMM[3]; } break;
        case REG4: { return XMM[4]; } break;
        case REG5: { return XMM[5]; } break;
        case REG6: { return XMM[6]; } break;
        case REG7: { return XMM[7]; } break;
        default:
    LxFatal("Invalid XMM register: %x\n", regNum);
    }
    Assert(0);
    return XMM[0];
}

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_SIMD_H__
