#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Not_F6_ext2(Processor *cpu, const Instruction *inst)
{
    /**
     * NOT r/m8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 r = ~val1;
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Not_F7_ext2(Processor *cpu, const Instruction *inst)
{
    /**
     * NOT r/m16
     * NOT r/m32
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    } else {
        u32 offset;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 r = ~val1;
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()