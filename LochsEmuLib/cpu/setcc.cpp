#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

static void SetByte(Processor *cpu, const Instruction *inst, bool cond)
{
    cpu->WriteOperand8(inst, inst->Main.Argument2, cpu->Offset32(inst->Main.Argument2), cond ? 1 : 0);
}

LxResult Seto_0F90(Processor *cpu, const Instruction *inst)
{
    /*
     * SETO r/m8
     */
    SetByte(cpu, inst, cpu->OF == 1);
    RET_SUCCESS();
}

LxResult Setno_0F91(Processor *cpu, const Instruction *inst)
{
    /*
     * SETNO r/m8
     */
    SetByte(cpu, inst, cpu->OF == 0);
    RET_SUCCESS();
}

LxResult Setc_0F92(Processor *cpu, const Instruction *inst)
{
    /*
     * SETC r/m8
     */
    SetByte(cpu, inst, cpu->CF == 1);
    RET_SUCCESS();
}

LxResult Setnb_0F93(Processor *cpu, const Instruction *inst)
{
    SetByte(cpu, inst, cpu->CF == 0);
    RET_SUCCESS();
}

LxResult Setz_0F94(Processor *cpu, const Instruction *inst)
{
    /**
     * SETZ r/m8
     */
    SetByte(cpu, inst, cpu->ZF == 1);
    RET_SUCCESS();
}

LxResult Setnz_0F95(Processor *cpu, const Instruction *inst)
{
    /**
     * SETNE r/m8
     */
    SetByte(cpu, inst, cpu->ZF == 0);
    RET_SUCCESS();
}

LxResult Setna_0F96(Processor *cpu, const Instruction *inst)
{
    SetByte(cpu, inst, cpu->CF == 1 || cpu->ZF == 1);
    RET_SUCCESS();
}

LxResult Seta_0F97(Processor *cpu, const Instruction *inst)
{
	/** 
	  *SETNBE r/m8
	  */
	SetByte(cpu, inst, cpu->CF == 0 && cpu->ZF == 0);
	RET_SUCCESS();
}

LxResult Sets_0F98(Processor *cpu, const Instruction *inst)
{
    SetByte(cpu, inst, cpu->SF == 1);
    RET_SUCCESS();
}

LxResult Setns_0F99(Processor *cpu, const Instruction *inst)
{
    SetByte(cpu, inst, cpu->SF == 0);
    RET_SUCCESS();
}

LxResult Setp_0F9A(Processor *cpu, const Instruction *inst)
{
    SetByte(cpu, inst, cpu->PF == 1);
    RET_SUCCESS();
}

LxResult Setpo_0F9B(Processor *cpu, const Instruction *inst)
{
    SetByte(cpu, inst, cpu->PF == 0);
    RET_SUCCESS();
}

LxResult Setl_0F9C(Processor *cpu, const Instruction *inst)
{
    SetByte(cpu, inst, cpu->SF != cpu->OF);
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

LxResult Setng_0F9E(Processor *cpu, const Instruction *inst)
{
    /**
     * SETLE r/m8
     */
    SetByte(cpu, inst, cpu->ZF == 1 || cpu->SF != cpu->OF);
    RET_SUCCESS();
}

LxResult Setnle_0F9F(Processor *cpu, const Instruction *inst)
{
    /*
     * SETNLE r/m8
     */
    SetByte(cpu, inst, cpu->ZF == 0 && cpu->SF == cpu->OF);
    RET_SUCCESS();
}


END_NAMESPACE_LOCHSEMU()