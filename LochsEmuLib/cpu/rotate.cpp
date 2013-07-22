#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define COUNT_MASK      0x1f

void Processor::Rcl8(u8 &a, u8 b)
{
    u8 r = a;
    b = (b & COUNT_MASK) % 9;
    r = (r << b) | (r >> (9-b));
    if (CF && b > 0) {
        r |= (1 << (b-1));
    }
    CF = (a & (1 << (8-b))) != 0;
    if (b == 1) {
        OF = (r >> 7) ^ CF;
    }
    a = r;
}

void Processor::Rcl16(u16 &a, u16 b)
{
	u16 r = a;
	b = (b & COUNT_MASK) %17;
	r = (r << b) | (r >> (17-b));
	if (CF && b > 0) {
		r |= (1 << (b-1));
	}
	CF = (a & (1 << (16-b))) != 0;
	if(b == 1) {
		OF = (r >> 15) ^ CF;
	}
	a = r;
}

void Processor::Rcl32(u32 &a, u8 b)
{
    u32 r = a;
    b = (b & COUNT_MASK) % 33;
    r = (r << b) | (r >> (33-b));
    if (CF && b > 0) {
        r |= (1 << (b-1));
    }
    CF = (a & (1 << (32-b))) != 0;
    if (b == 1) {
        OF = (r >> 31) ^ CF;
    }
    a = r;
}

void Processor::Rol32(u32 &a, u8 b)
{
    b &= COUNT_MASK;
    u32 r = (a << b) | (a >> (32-b));
    if (r & 1) {
        CF = 1;
    }
    if (b == 1) {
        OF = (MSB32(r) ^ CF) ? 1 : 0;
    }
    a = r;
}

void Processor::Rol16(u16 &a, u8 b)
{
    b &= COUNT_MASK;
    u16 r = (a << b) | (a >> (16-b));
    if (r & 1) {
        CF = 1;
    }
    if (b == 1) {
        OF = (MSB16(r) ^ CF) ? 1 : 0;
    }
    a = r;
}

void Processor::Rol8(u8 &a, u8 b)
{
    b &= COUNT_MASK;
    u8 r = (a << b) | (a >> (8-b));
    if (r & 1) CF = 1;
    if (b == 1) {
        OF = (MSB8(r) ^ CF) ? 1 : 0;
    }
    a = r;
}

void Processor::Ror8(u8 &a, u8 b)
{
    b &= COUNT_MASK;
    u8 r = (a >> b) | (a << (8-b));
    if (MSB8(r)) CF = 1;
    if (b == 1) {
        OF = ((r >> 7) ^ ((r >> 6) & 1)) ? 1 : 0;
    }
    a = r;
}

void Processor::Ror16(u16 &a, u8 b)
{
    b &= COUNT_MASK;
    u16 r = (a >> b) | (a << (16-b));
    if (MSB16(r)) CF = 1;
    if (b == 1) {
        OF = ((r >> 15) ^ ((r >> 14) & 1)) ? 1 : 0;
    }
    a = r;
}

void Processor::Ror32(u32 &a, u8 b)
{
    b &= COUNT_MASK;
    u32 r = (a >> b) | (a << (32-b));
    if (MSB32(r)) CF = 1;
    if (b == 1) {
        OF = ((r >> 31) ^ ((r >> 30) & 1)) ? 1 : 0;
    }
    a = r;

}

void Processor::Rol_C0_ext0(const Instruction *inst)
{
    // ROL r/m8, imm8
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Rol8(val1, val2);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Ror_C0_ext1(const Instruction *inst)
{
    // ROR r/m8, imm8
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Ror8(val1, val2);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Rol_C1_ext0(const Instruction *inst)
{
    // ROL r/m32, imm8
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Rol16(val1, val2);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u8 val2 = (u8) inst->Main.Inst.Immediat;
        Rol32(val1, val2);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
}

void Processor::Ror_C1_ext1(const Instruction *inst)
{
    // ROR r/m32, imm8
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    u32 offset;
    u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Ror32(val1, val2);
    WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Rol_D0_ext0(const Instruction *inst)
{
    // ROL r/m8, 1
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    Rol8(val1, 1);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Rcl_D0_ext2(const Instruction *inst)
{
    // RCL r/m8, 1
    u32 offset;
    u8 val1 = ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Rcl8(val1, val2);
    WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Rol_D1_ext0(const Instruction *inst)
{
    // ROL r/m32, 1
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        NOT_IMPLEMENTED();
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Rol32(val1, 1);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }

    
}

void Processor::Ror_D1_ext1(const Instruction *inst)
{
    // ROR r/m32, 1
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        Ror16(val1, 1);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Ror32(val1, 1);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
}

void Processor::Rcl_D1_ext2(const Instruction *inst)
{
	// RCL r/m16, 1
	u32 offset;

    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        Rcl16(val1, 1);
        WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        Rcl32(val1, 1);
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }

	
}

void Processor::Rol_D3_ext0(const Instruction *inst)
{
    // ROL r/m32, cl
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    u32 offset;
    u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
    Rol32(val1, CL);
    WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    
}

void Processor::Rcl_D3_ext2(const Instruction *inst)
{
    // RCL r/m32, cl
    u32 offset;
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
    Rcl32(val1, CL);
    WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    
}

END_NAMESPACE_LOCHSEMU()