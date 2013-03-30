#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Movsb_A4(const Instruction *inst)
{
    // MOVSB
    if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();

    u8 temp = MemRead8(ESI, LX_REG_DS);
    MemWrite8(EDI, temp, LX_REG_ES);
    if (DF == 0) {
        EDI++; ESI++;
    } else {
        EDI--; ESI--;
    }
    
}

void Processor::Movs_A5(const Instruction *inst)
{
    if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();

    if (inst->Main.Prefix.OperandSize) { // 16-bit mode
        u16 temp = MemRead16(ESI, LX_REG_DS);
        MemWrite16(EDI, temp, LX_REG_ES);
        if (DF == 0) {
            EDI += 2; ESI += 2;
        } else {
            EDI -= 2; EDI -= 2;
        }
    } else { // 32-bit mode
        u32 temp = MemRead32(ESI, LX_REG_DS);
        MemWrite32(EDI, temp, LX_REG_ES);
        if (DF == 0) {
            EDI += 4; ESI += 4;
        } else {
            EDI -= 4; EDI -= 4;
        }
    }
    
}

void Processor::Cmpsb_A6(const Instruction *inst)
{
	if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();
	
	u8 tmp1 = MemRead8(ESI, LX_REG_DS);
	u8 tmp2 = MemRead8(EDI, LX_REG_ES);   // used to be a terrible bug
	u8 tmp = tmp1 - tmp2;
	SetFlagsArith8(tmp, PROMOTE_U16(tmp1) - PROMOTE_U16(tmp2),
		PROMOTE_I16(tmp1) - PROMOTE_I16(tmp2));
	if (DF == 0) {
		ESI++; EDI++;
	} else {
		ESI--; EDI--;
	}
	
}

void Processor::Cmpsd_A7(const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        u16 tmp1 = MemRead16(ESI, LX_REG_DS);
        u16 tmp2 = MemRead16(EDI, LX_REG_ES);
        u16 tmp = tmp1 - tmp2;
        SetFlagsArith16(tmp, PROMOTE_U32(tmp1) - PROMOTE_U32(tmp2),
            PROMOTE_I32(tmp1) - PROMOTE_I32(tmp2));
        if (DF == 0) {
            ESI += 2; EDI += 2;
        } else {
            ESI -= 2; EDI -= 2;
        }
    } else {
        u32 tmp1 = MemRead32(ESI, LX_REG_DS);
        u32 tmp2 = MemRead32(EDI, LX_REG_ES);
        u32 tmp = tmp1 - tmp2;
        SetFlagsArith32(tmp, PROMOTE_U64(tmp1) - PROMOTE_U64(tmp2),
            PROMOTE_I64(tmp1) - PROMOTE_I64(tmp2));
        if (DF == 0) {
            ESI += 4; EDI += 4;
        } else {
            ESI -= 4; EDI -= 4;
        }
    }
    
}

void Processor::Stos_AA(const Instruction *inst)
{
    if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    MemWrite8(EDI, AL, LX_REG_ES);
    DF == 0 ? EDI++ : EDI--;
    
}

void Processor::Stos_AB(const Instruction *inst)
{
    // STOS
    if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();
    if (inst->Main.Prefix.OperandSize) {
        MemWrite16(EDI, AX, LX_REG_ES);
        DF == 0 ? EDI += 2 : EDI -= 2;
    } else {
        MemWrite32(EDI, EAX, LX_REG_ES);
        DF == 0 ? EDI += 4 : EDI -= 4;
    }
    
}

void Processor::Lodsb_AC(const Instruction *inst)
{
    // LODSB
    if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();
    AL = MemRead8(ESI);
    DF == 0 ? ESI++ : ESI--;
    
}

void Processor::Lodsd_AD(const Instruction *inst)
{
    /**
     * mov EAX,DS:ESI
	 then IF(DF=0)esi¡ûesi+4
	 else esi¡ûesi-4; 
     */
    if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    EAX = MemRead32(ESI);
	if (!DF) 
        ESI += 4;
	else 
        ESI -= 4;
    
}


void Processor::Scasb_AE(const Instruction *inst)
{
    // SCASB
    if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();
    u8 val1 = AL;
    u8 val2 = MemRead8(EDI, LX_REG_ES);
    u8 r = val1 - val2;
    SetFlagsArith8(r, PROMOTE_U16(val1) - PROMOTE_U16(val2),
        PROMOTE_I16(val1) - PROMOTE_I16(val2));
    if (DF == 0) {
        EDI++; 
    } else {
        EDI--;
    }
    
}



END_NAMESPACE_LOCHSEMU()