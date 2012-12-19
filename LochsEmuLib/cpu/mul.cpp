#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Mul_F6_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * MUL r/m8
     */
    u8 al = cpu->AL;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument2, NULL);
    u16 r = (u16) al * (u16) val1;
    cpu->AX = r;
    cpu->CF = cpu->AH == 0 ? 0 : 1;
    cpu->OF = cpu->CF;
    RET_SUCCESS();
}

LxResult Mul_F7_ext4(Processor *cpu, const Instruction *inst)
{
    /**
     * MUL r/m16
     * MUL r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 eax = cpu->EAX;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
        u64 r = (u64) eax * (u64) val1;
        cpu->EAX = (u32) r;
        cpu->EDX = (u32) (r >> 32);
        cpu->CF = cpu->EDX == 0 ? 0 : 1;
        cpu->OF = cpu->CF;
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()