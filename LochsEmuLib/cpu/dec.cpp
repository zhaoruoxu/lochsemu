#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Dec_48X(const Instruction *inst)
{
    /**
     * DEC r32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 val1 = GP_Regs[inst->Main.Inst.Opcode - 0x48].X32;
    u32 r = val1 - 1;
    SetFlagsShift32(r);
    SetFlagOF32(r, PROMOTE_I64(val1) - PROMOTE_I64(1));
    GP_Regs[inst->Main.Inst.Opcode - 0x48].X32 = r;

    
}
void Processor::Dec_FE_ext1(const Instruction *inst)
{
   /**
    * DEC r/m8
    */
   u32 offset;
   u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
   u8 r = val1 - 1;
   SetFlagsShift8(r);
   SetFlagOF8(r, PROMOTE_I16(val1) - PROMOTE_I16(1));
   WriteOperand8(inst, inst->Main.Argument1, offset, r);
   
}
void Processor::Dec_FF_ext1(const Instruction *inst)
{
    /**
     * DEC r/m32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 offset;
    u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
    u32 r = val1 - 1;
    SetFlagsShift32(r);
    SetFlagOF32(r, PROMOTE_I64(val1) - PROMOTE_I64(1));
    WriteOperand32(inst, inst->Main.Argument1, offset, r);
    
}


END_NAMESPACE_LOCHSEMU()