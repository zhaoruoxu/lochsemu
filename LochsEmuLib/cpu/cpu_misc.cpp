#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Bound_62(Processor *cpu, const Instruction *inst)
{
    // do nothing for now
    RET_SUCCESS();
}

LxResult Sahf_9E(Processor *cpu, const Instruction *inst)
{
    // SAHF
    u16 ah = cpu->AH;
    cpu->CF =    (ah & 1);
    cpu->PF =    (ah >> 2) & 1;
    cpu->AF =    (ah >> 4) & 1;
    cpu->ZF =    (ah >> 6) & 1;
    cpu->SF =    (ah >> 7) & 1;
    RET_SUCCESS();
}

LxResult Nop_90(Processor *cpu, const Instruction *inst)
{
    // NOP
    RET_SUCCESS();
}

LxResult Xgetbv_0F01(Processor *cpu, const Instruction *inst)
{
    // XGETBV
    u32 a, d;
    __asm {
        xor ecx, ecx
        xgetbv
        mov a, eax
        mov d, edx
    }
    cpu->EAX = a;
    cpu->EDX = d;
    RET_SUCCESS();
}

LxResult Nop_0F1F_0(Processor *cpu, const Instruction *inst)
{
    // NOP
    RET_SUCCESS();
}

LxResult Rdtsc_0F31(Processor *cpu, const Instruction *inst)
{
    // RDTSC
    u32 a, d;
    __asm {
        rdtsc
        mov a, eax
        mov d, edx
    }
    cpu->EAX = a;
    cpu->EDX = d;
    RET_SUCCESS();
}

LxResult Cpuid_0FA2(Processor *cpu, const Instruction *inst)
{
    /**
     * CPUID
     */
    u32 a, b, c, d;
    u32 emuEax = cpu->EAX;
    __asm {
        mov eax, emuEax
        cpuid
        mov a, eax
        mov b, ebx
        mov c, ecx
        mov d, edx
    }

//     if (cpu->EAX == 1) {
//         d &= 0xf97fffff;
//     }

    cpu->EAX = a;
    cpu->EBX = b;
    cpu->ECX = c;
    cpu->EDX = d;
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()