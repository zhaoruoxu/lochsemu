#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Bound_62(const Instruction *inst)
{
    // do nothing for now
    
}

void Processor::Sahf_9E(const Instruction *inst)
{
    // SAHF
    u16 ah = AH;
    CF =    (ah & 1);
    PF =    (ah >> 2) & 1;
    AF =    (ah >> 4) & 1;
    ZF =    (ah >> 6) & 1;
    SF =    (ah >> 7) & 1;
    
}

void Processor::Nop_90(const Instruction *inst)
{
    // NOP
    
}

void Processor::Xgetbv_0F01(const Instruction *inst)
{
    // XGETBV
    u32 a, d;
    __asm {
        xor ecx, ecx
        xgetbv
        mov a, eax
        mov d, edx
    }
    EAX = a;
    EDX = d;
    
}

void Processor::Nop_0F1F_0(const Instruction *inst)
{
    // NOP
    
}

void Processor::Rdtsc_0F31(const Instruction *inst)
{
    // RDTSC
    u32 a, d;
    __asm {
        rdtsc
        mov a, eax
        mov d, edx
    }
    EAX = a;
    EDX = d;
    
}

void Processor::Cpuid_0FA2(const Instruction *inst)
{
    /**
     * CPUID
     */
    u32 a, b, c, d;
    u32 emuEax = EAX;
    u32 emuEcx = ECX;
    __asm {
        mov eax, emuEax
        mov ecx, emuEcx
        cpuid
        mov a, eax
        mov b, ebx
        mov c, ecx
        mov d, edx
    }

//     if (EAX == 1) {
//         d &= 0xf97fffff;
//     }

    if (EAX == 1) {
        c &= ~( (1 << 20) | (1 << 19) | (1 << 9) | 1);  // SSE4.2 SSE4.1 SSSE3 SSE3
    }

    EAX = a;
    EBX = b;
    ECX = c;
    EDX = d;
    
}

END_NAMESPACE_LOCHSEMU()