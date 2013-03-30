#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define COUNT_MASK      0x1f

void Processor::Sar32(u32 &a, u8 b)
{
    i32 r = (i32) a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            OF = 0;
        }
        a = (u32) r;
        SetFlagsShift32(r);
    }
}

void Processor::Sar16(u16 &a, u8 b)
{
    i16 r = (i16) a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) { 
            OF = 0; 
        }
        a = (u32) r;
        SetFlagsShift32(r);
    }
}

void Processor::Sar8(u8 &a, u8 b)
{
    i8 r = (i8) a;
    b &= COUNT_MASK;
    if (b > 0) {
        CF = (r & (1 << (b-1))) ? 1 : 0;
        r >>= b;
        if (b == 1) {
            OF = 0;
        }
        a = (u8) r;
        SetFlagsShift8(r);
    }
}

void Processor::Sar_C0_ext7(const Instruction *inst)
{
    /**
     * SAR r/m8, imm8
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Sar8(val1, val2);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Sar_C1_ext7(const Instruction *inst)
{
    /**
     * SAR r/m16, imm8
     * SAR r/m32, imm8
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Sar16(val1, val2);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Sar32(val1, val2);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

void Processor::Sar_D0_ext7(const Instruction *inst)
{
    /**
     * SAR r/m8, 1
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    Sar8(val1, 1);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Sar_D1_ext7(const Instruction *inst)
{
    /**
     * SAR r/m16, 1
     * SAR r/m32, 1
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        Sar16(val1, 1);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Sar32(val1, 1);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

void Processor::Sar_D2_ext7(const Instruction *inst)
{
    /**
     * SAR r/m8, CL
     */
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    Sar8(val1, CL);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Sar_D3_ext7(const Instruction *inst)
{
    /**
     * SAR r/m16, CL
     * SAR r/m32, CL
     */
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        Sar16(val1, CL);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Sar32(val1, CL);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
    
}

END_NAMESPACE_LOCHSEMU()