#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Inc_40X(const Instruction *inst)
{
    /**
     * INC r16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = GP_Regs[inst->Main.Inst.Opcode - 0x40].X16;
        u16 r = val1 + 1;
        SetFlagsShift16(r);
        SetFlagOF16(r, PROMOTE_I32(val1) + PROMOTE_I32(1));
        GP_Regs[inst->Main.Inst.Opcode - 0x40].X16 = r;
    } else {
        u32 val1 = GP_Regs[inst->Main.Inst.Opcode - 0x40].X32;
        u32 r = val1 + 1;
        SetFlagsShift32(r); // ZF, PF, SF
        SetFlagOF32(r, PROMOTE_I64(val1) + PROMOTE_I64(1));
        GP_Regs[inst->Main.Inst.Opcode - 0x40].X32 = r;
    }
    
}

void Processor::Inc_FE_ext0(const Instruction *inst)
{
    /**
     * INC r/m8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 r = val1 + 1;
    SetFlagsShift8(r);
    SetFlagOF8(r, PROMOTE_I16(val1) + PROMOTE_I16(1));
    WriteOperand8(inst, inst->Main.Argument1, offset, r);
    
}

void Processor::Inc_FF_ext0(const Instruction *inst)
{
    /**
     * INC r/m32
     * INC r/m16
     */
    if (inst->Main.Prefix.OperandSize) {
        u32 offset;
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 r = val1 + 1;
        SetFlagsShift16(r);
        SetFlagOF16(r, PROMOTE_I32(val1) + PROMOTE_I32(1));
        WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 offset;
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 r = val1 + 1;
        SetFlagsShift32(r);
        SetFlagOF32(r, PROMOTE_I64(val1) + PROMOTE_I64(1));
        WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    
}

END_NAMESPACE_LOCHSEMU()