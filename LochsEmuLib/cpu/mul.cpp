#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Mul_F6_ext4(const Instruction *inst)
{
    /**
     * MUL r/m8
     */
    u8 al = AL;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument2, NULL);
    u16 r = (u16) al * (u16) val1;
    AX = r;
    CF = AH == 0 ? 0 : 1;
    OF = CF;
    
}

void Processor::Mul_F7_ext4(const Instruction *inst)
{
    /**
     * MUL r/m16
     * MUL r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        NOT_IMPLEMENTED();
    } else {
        u32 eax = EAX;
        u32 val1 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u64 r = (u64) eax * (u64) val1;
        EAX = (u32) r;
        EDX = (u32) (r >> 32);
        CF = EDX == 0 ? 0 : 1;
        OF = CF;
    }
    
}

END_NAMESPACE_LOCHSEMU()