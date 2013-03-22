#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define COUNT_MASK      0x1f

static INLINE void Rcl8(Processor *cpu, u8 &a, u8 b)
{
    u8 r = a;
    b = (b & COUNT_MASK) % 9;
    r = (r << b) | (r >> (9-b));
    if (cpu->CF && b > 0) {
        r |= (1 << (b-1));
    }
    cpu->CF = (a & (1 << (8-b))) != 0;
    if (b == 1) {
        cpu->OF = (r >> 7) ^ cpu->CF;
    }
    a = r;
}

static INLINE void Rcl16(Processor *cpu, u16 &a, u16 b)
{
	u16 r = a;
	b = (b & COUNT_MASK) %17;
	r = (r << b) | (r >> (17-b));
	if (cpu->CF && b > 0) {
		r |= (1 << (b-1));
	}
	cpu->CF = (a & (1 << (16-b))) != 0;
	if(b == 1) {
		cpu->OF = (r >> 15) ^ cpu->CF;
	}
	a = r;
}

static INLINE void Rcl32(Processor *cpu, u32 &a, u8 b)
{
    u32 r = a;
    b = (b & COUNT_MASK) % 33;
    r = (r << b) | (r >> (33-b));
    if (cpu->CF && b > 0) {
        r |= (1 << (b-1));
    }
    cpu->CF = (a & (1 << (32-b))) != 0;
    if (b == 1) {
        cpu->OF = (r >> 31) ^ cpu->CF;
    }
    a = r;
}

static INLINE void Rol32(Processor *cpu, u32 &a, u8 b)
{
    b &= COUNT_MASK;
    u32 r = (a << b) | (a >> (32-b));
    if (r & 1) {
        cpu->CF = 1;
    }
    if (b == 1) {
        cpu->OF = (MSB32(r) ^ cpu->CF) ? 1 : 0;
    }
    a = r;
}

static INLINE void Rol8(Processor *cpu, u8 &a, u8 b)
{
    b &= COUNT_MASK;
    u8 r = (a << b) | (a >> (8-b));
    if (r & 1) cpu->CF = 1;
    if (b == 1) {
        cpu->OF = (MSB8(r) ^ cpu->CF) ? 1 : 0;
    }
    a = r;
}

static INLINE void Ror8(Processor *cpu, u8 &a, u8 b)
{
    b &= COUNT_MASK;
    u8 r = (a >> b) | (a << (8-b));
    if (MSB8(r)) cpu->CF = 1;
    if (b == 1) {
        cpu->OF = ((r >> 7) ^ ((r >> 6) & 1)) ? 1 : 0;
    }
    a = r;
}

static INLINE void Ror32(Processor *cpu, u32 &a, u8 b)
{
    b &= COUNT_MASK;
    u32 r = (a >> b) | (a << (32-b));
    if (MSB32(r)) cpu->CF = 1;
    if (b == 1) {
        cpu->OF = ((r >> 31) ^ ((r >> 30) & 1)) ? 1 : 0;
    }
    a = r;

}

LxResult Rol_C0_ext0(Processor *cpu, const Instruction *inst)
{
    // ROL r/m8, imm8
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Rol8(cpu, val1, val2);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Ror_C0_ext1(Processor *cpu, const Instruction *inst)
{
    // ROR r/m8, imm8
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Ror8(cpu, val1, val2);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Rol_C1_ext0(Processor *cpu, const Instruction *inst)
{
    // ROL r/m32, imm8
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 offset;
    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Rol32(cpu, val1, val2);
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Ror_C1_ext1(Processor *cpu, const Instruction *inst)
{
    // ROR r/m32, imm8
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    u32 offset;
    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Ror32(cpu, val1, val2);
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Rol_D0_ext0(Processor *cpu, const Instruction *inst)
{
    // ROL r/m8, 1
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    Rol8(cpu, val1, 1);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Rcl_D0_ext2(Processor *cpu, const Instruction *inst)
{
    // RCL r/m8, 1
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 val2 = (u8) inst->Main.Inst.Immediat;
    Rcl8(cpu, val1, val2);
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Rol_D1_ext0(Processor *cpu, const Instruction *inst)
{
    // ROL r/m32, 1
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        RET_NOT_IMPLEMENTED();
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        Rol32(cpu, val1, 1);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }

    RET_SUCCESS();
}

LxResult Rcl_D1_ext2(Processor *cpu, const Instruction *inst)
{
	// RCL r/m16, 1
	u32 offset;

    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        Rcl16(cpu, val1, 1);
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, val1);
    } else {
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        Rcl32(cpu, val1, 1);
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }

	RET_SUCCESS();
}

LxResult Rol_D3_ext0(Processor *cpu, const Instruction *inst)
{
    // ROL r/m32, cl
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    u32 offset;
    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    Rol32(cpu, val1, cpu->CL);
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

LxResult Rcl_D3_ext2(Processor *cpu, const Instruction *inst)
{
    // RCL r/m32, cl
    u32 offset;
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
    Rcl32(cpu, val1, cpu->CL);
    cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()