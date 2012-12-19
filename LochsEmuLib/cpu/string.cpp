#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Movsb_A4(Processor *cpu, const Instruction *inst)
{
    // MOVSB
    if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();

    u8 temp = cpu->MemRead8(cpu->ESI, LX_REG_DS);
    cpu->MemWrite8(cpu->EDI, temp, LX_REG_ES);
    if (cpu->DF == 0) {
        cpu->EDI++; cpu->ESI++;
    } else {
        cpu->EDI--; cpu->ESI--;
    }
    RET_SUCCESS();
}

LxResult Movs_A5(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();

    if (inst->Main.Prefix.OperandSize) { // 16-bit mode
        u16 temp = cpu->MemRead16(cpu->ESI, LX_REG_DS);
        cpu->MemWrite16(cpu->EDI, temp, LX_REG_ES);
        if (cpu->DF == 0) {
            cpu->EDI += 2; cpu->ESI += 2;
        } else {
            cpu->EDI -= 2; cpu->EDI -= 2;
        }
    } else { // 32-bit mode
        u32 temp = cpu->MemRead32(cpu->ESI, LX_REG_DS);
        cpu->MemWrite32(cpu->EDI, temp, LX_REG_ES);
        if (cpu->DF == 0) {
            cpu->EDI += 4; cpu->ESI += 4;
        } else {
            cpu->EDI -= 4; cpu->EDI -= 4;
        }
    }
    RET_SUCCESS();
}

LxResult CMPSB_A6(Processor *cpu, const Instruction *inst)
{
	if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();
	
	u8 tmp1 = cpu->MemRead8(cpu->ESI, LX_REG_DS);
	u8 tmp2 = cpu->MemRead8(cpu->ESI, LX_REG_ES);
	u8 tmp = tmp1 - tmp2;
	cpu->SetFlagsArith8(tmp, PROMOTE_U16(tmp1) - PROMOTE_U16(tmp2),
		PROMOTE_I16(tmp1) - PROMOTE_I16(tmp2));
	if(cpu->DF == 0) {
		cpu->ESI++; cpu->EDI++;
	} else {
		cpu->ESI--; cpu->EDI--;
	}
	RET_SUCCESS();
}

LxResult Stos_AA(Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();

    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    cpu->MemWrite8(cpu->EDI, cpu->AL, LX_REG_ES);
    cpu->DF == 0 ? cpu->EDI++ : cpu->EDI--;
    RET_SUCCESS();
}

LxResult Stos_AB(Processor *cpu, const Instruction *inst)
{
    // STOS
    if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();
    if (inst->Main.Prefix.OperandSize) {
        cpu->MemWrite16(cpu->EDI, cpu->AX, LX_REG_ES);
        cpu->DF == 0 ? cpu->EDI +=2 : cpu->EDI -= 2;
    } else {
        cpu->MemWrite32(cpu->EDI, cpu->EAX, LX_REG_ES);
        cpu->DF == 0 ? cpu->EDI += 4 : cpu->EDI -= 4;
    }
    RET_SUCCESS();
}

LxResult Lodsb_AC(Processor *cpu, const Instruction *inst)
{
    // LODSB
    if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();
    cpu->AL = cpu->MemRead8(cpu->ESI);
    cpu->DF == 0 ? cpu->ESI++ : cpu->ESI--;
    RET_SUCCESS();
}

LxResult Lodsd_AD(Processor *cpu, const Instruction *inst)
{
    /**
     * mov EAX,DS:ESI
	 then IF(DF=0)esi¡ûesi+4
	 else esi¡ûesi-4; 
     */
    if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    cpu->EAX = cpu->MemRead32(cpu->ESI);
	if (!cpu->DF) 
        cpu->ESI += 4;
	else 
        cpu->ESI -= 4;
    RET_SUCCESS();
}


LxResult Scasb_AE(Processor *cpu, const Instruction *inst)
{
    // SCASB
    if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();
    u8 val1 = cpu->AL;
    u8 val2 = cpu->MemRead8(cpu->EDI, LX_REG_ES);
    u8 r = val1 - val2;
    cpu->SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    if (cpu->DF == 0) {
        cpu->EDI++; 
    } else {
        cpu->EDI--;
    }
    RET_SUCCESS();
}



END_NAMESPACE_LOCHSEMU()