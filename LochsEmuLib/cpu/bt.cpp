#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Bt_0FA3(const Instruction *inst)
{
	// BT r/m16/32, r16/32
	u32 offset;
	if (inst->Main.Prefix.OperandSize) {
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 val = (val >> val2) & 1;
		CF = val;
	}
	else{
		u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
		u32 val = (val1 >> val2) & 1;
		CF = val;
	}
	
}

void Processor::Bt_0FBA_4(const Instruction* inst)
{
	// BT r/m16/32, imm8
	u32 offset;
	if (inst->Main.Prefix.OperandSize) {
		u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = (u16) inst->Main.Inst.Immediat;
		u16 val = (val1 >> val2) & 1;
		CF = val;
	}
	else{
		u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = (u32) inst->Main.Inst.Immediat;
		u32 val = (val1 >> val2) & 1;
		CF = val;
	}
	
}

void Processor::Bswap_0FC8(const Instruction *inst)
{
	// BSWAP r32
	u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
	u32 val = val1 >> 24;
	u32 tmp = (val1 >> 8) & 0xff00;
	val = val | tmp;
	tmp = (val1 << 8) & 0xff0000;
	val = val | tmp;
	tmp = val1 << 24;
	val = val | tmp;
	WriteOperand32(inst, inst->Main.Argument1, NULL, val);
	
}

END_NAMESPACE_LOCHSEMU()