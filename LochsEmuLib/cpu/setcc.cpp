#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::SetByte(const Instruction *inst, bool cond)
{
    WriteOperand8(inst, inst->Main.Argument2, Offset32(inst->Main.Argument2), cond ? 1 : 0);
}

void Processor::Seto_0F90(const Instruction *inst)
{
    /*
     * SETO r/m8
     */
    SetByte(inst, OF == 1);
    
}

void Processor::Setno_0F91(const Instruction *inst)
{
    /*
     * SETNO r/m8
     */
    SetByte(inst, OF == 0);
    
}

void Processor::Setc_0F92(const Instruction *inst)
{
    /*
     * SETC r/m8
     */
    SetByte(inst, CF == 1);
    
}

void Processor::Setnb_0F93(const Instruction *inst)
{
    SetByte(inst, CF == 0);
    
}

void Processor::Setz_0F94(const Instruction *inst)
{
    /**
     * SETZ r/m8
     */
    SetByte(inst, ZF == 1);
    
}

void Processor::Setnz_0F95(const Instruction *inst)
{
    /**
     * SETNE r/m8
     */
    SetByte(inst, ZF == 0);
    
}

void Processor::Setna_0F96(const Instruction *inst)
{
    SetByte(inst, CF == 1 || ZF == 1);
    
}

void Processor::Seta_0F97(const Instruction *inst)
{
	/** 
	  *SETNBE r/m8
	  */
	SetByte(inst, CF == 0 && ZF == 0);
	
}

void Processor::Sets_0F98(const Instruction *inst)
{
    SetByte(inst, SF == 1);
    
}

void Processor::Setns_0F99(const Instruction *inst)
{
    SetByte(inst, SF == 0);
    
}

void Processor::Setp_0F9A(const Instruction *inst)
{
    SetByte(inst, PF == 1);
    
}

void Processor::Setpo_0F9B(const Instruction *inst)
{
    SetByte(inst, PF == 0);
    
}

void Processor::Setl_0F9C(const Instruction *inst)
{
    SetByte(inst, SF != OF);
    
}

void Processor::Setnl_0F9D(const Instruction *inst)
{
    /*
     * SETNL r/m8
     */
    SetByte(inst, SF == OF);
    
}

void Processor::Setng_0F9E(const Instruction *inst)
{
    /**
     * SETLE r/m8
     */
    SetByte(inst, ZF == 1 || SF != OF);
    
}

void Processor::Setnle_0F9F(const Instruction *inst)
{
    /*
     * SETNLE r/m8
     */
    SetByte(inst, ZF == 0 && SF == OF);
    
}


END_NAMESPACE_LOCHSEMU()