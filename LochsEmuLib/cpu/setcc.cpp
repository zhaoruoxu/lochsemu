#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

static void SetByte(Processor *cpu, const Instruction *inst, bool cond)
{
    cpu->WriteOperand8(inst, inst->Main.Argument2, cpu->Offset32(inst->Main.Argument2), cond ? 1 : 0);
}

LxResult Setz_0F94(Processor *cpu, const Instruction *inst)
{
    /**
     * SETZ r/m8
     */
    SetByte(cpu, inst, cpu->ZF == 1);
    RET_SUCCESS();
}

LxResult Setne_0F95(Processor *cpu, const Instruction *inst)
{
    /**
     * SETNE r/m8
     */
    SetByte(cpu, inst, cpu->ZF == 0);
    RET_SUCCESS();
}

LxResult Setnbe_0F97(Processor *cpu, const Instruction *inst)
{
	/** 
	  *SETNBE r/m8
	  */
	SetByte(cpu, inst, cpu->CF == 0 && cpu->ZF == 0);
	RET_SUCCESS();
}

LxResult Setnl_0F9D(Processor *cpu, const Instruction *inst)
{
    /*
     * SETNL r/m8
     */
    SetByte(cpu, inst, cpu->SF == cpu->OF);
    RET_SUCCESS();
}

LxResult Setle_0F9E(Processor *cpu, const Instruction *inst)
{
    /**
     * SETLE r/m8
     */
    SetByte(cpu, inst, cpu->ZF == 1 || cpu->SF != cpu->OF);
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()