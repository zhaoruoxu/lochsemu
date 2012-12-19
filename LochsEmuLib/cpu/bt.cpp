#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Bt_0FA3(Processor *cpu, const Instruction *inst)
{
	// BT r/m16/32, r16/32
	u32 offset;
	if (inst->Main.Prefix.OperandSize) {
		u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
		u16 val = (val >> val2) & 1;
		cpu->CF = val;
	}
	else{
		u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
		u32 val = (val1 >> val2) & 1;
		cpu->CF = val;
	}
	RET_SUCCESS();
}

LxResult Bt_0FBA_4(Processor *cpu, const Instruction* inst)
{
	// BT r/m16/32, imm8
	u32 offset;
	if (inst->Main.Prefix.OperandSize) {
		u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
		u16 val2 = (u16) inst->Main.Inst.Immediat;
		u16 val = (val1 >> val2) & 1;
		cpu->CF = val;
	}
	else{
		u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
		u32 val2 = (u32) inst->Main.Inst.Immediat;
		u32 val = (val1 >> val2) & 1;
		cpu->CF = val;
	}
	RET_SUCCESS();
}

LxResult Bswap_0FC8(Processor *cpu, const Instruction *inst)
{
	// BSWAP r32
	u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, NULL);
	u32 val = val1 >> 24;
	u32 tmp = (val1 >> 8) & 0xff00;
	val = val | tmp;
	tmp = (val1 << 8) & 0xff0000;
	val = val | tmp;
	tmp = val1 << 24;
	val = val | tmp;
	cpu->WriteOperand32(inst, inst->Main.Argument1, NULL, val);
	RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()