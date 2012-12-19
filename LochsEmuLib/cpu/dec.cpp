#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Dec_48X(Processor *cpu, const Instruction *inst)
{
    /**
     * DEC r32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 val1 = cpu->GP_Regs[inst->Main.Inst.Opcode - 0x48].X32;
    u32 r = val1 - 1;
    cpu->SetFlagsShift32(r);
    cpu->SetFlagOF32(r, PROMOTE_I64(val1) - PROMOTE_I64(1));
    cpu->GP_Regs[inst->Main.Inst.Opcode - 0x48].X32 = r;

    RET_SUCCESS();
}
LxResult Dec_FE_ext1(Processor *cpu, const Instruction *inst)
{
   /**
    * DEC r/m8
    */
   u32 offset;
   u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
   u8 r = val1 - 1;
   cpu->SetFlagsShift8(r);
   cpu->SetFlagOF8(r, PROMOTE_I16(val1) - PROMOTE_I16(1));
   cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
   RET_SUCCESS();
}
LxResult Dec_FF_ext1(Processor *cpu, const Instruction *inst)
{
    /**
     * DEC r/m32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 offset;
    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 r = val1 - 1;
    cpu->SetFlagsShift32(r);
    cpu->SetFlagOF32(r, PROMOTE_I64(val1) - PROMOTE_I64(1));
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}


END_NAMESPACE_LOCHSEMU()