#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Ext_C6(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Mov_C6_ext0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::InstNotAvailable,
        /* 0x5 */ &Processor::InstNotAvailable,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_C7(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Mov_C7_ext0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::InstNotAvailable,
        /* 0x5 */ &Processor::InstNotAvailable,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_80(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Add_80_ext0,
        /* 0x1 */ &Processor::Or_80_ext1,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::And_80_ext4,
        /* 0x5 */ &Processor::Sub_80_ext5,
        /* 0x6 */ &Processor::Xor_80_ext6,
        /* 0x7 */ &Processor::Cmp_80_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_81(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Add_81_ext0,
        /* 0x1 */ &Processor::Or_81_ext1,
        /* 0x2 */ &Processor::Adc_81_ex2,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::And_81_ext4,
        /* 0x5 */ &Processor::Sub_81_ext5,
        /* 0x6 */ &Processor::Xor_81_ext6,
        /* 0x7 */ &Processor::Cmp_81_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_83(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Add_83_ext0,
        /* 0x1 */ &Processor::Or_83_ext1,
        /* 0x2 */ &Processor::Adc_83_ext2,
        /* 0x3 */ &Processor::Sbb_83_ext3,
        /* 0x4 */ &Processor::And_83_ext4,
        /* 0x5 */ &Processor::Sub_83_ext5,
        /* 0x6 */ &Processor::Xor_83_ext6,
        /* 0x7 */ &Processor::Cmp_83_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_8F(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Pop_8F_ext0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::InstNotAvailable,
        /* 0x5 */ &Processor::InstNotAvailable,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_C0(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Rol_C0_ext0,
        /* 0x1 */ &Processor::Ror_C0_ext1,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::Shl_C0_ext4,
        /* 0x5 */ &Processor::Shr_C0_ext5,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::Sar_C0_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_C1(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Rol_C1_ext0,
        /* 0x1 */ &Processor::Ror_C1_ext1,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::Shl_C1_ext4,
        /* 0x5 */ &Processor::Shr_C1_ext5,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::Sar_C1_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_D0(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Rol_D0_ext0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::Rcl_D0_ext2,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::Shl_D0_ext4,
        /* 0x5 */ &Processor::Shr_D0_ext5,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::Sar_D0_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_D1(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Rol_D1_ext0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::Rcl_D1_ext2,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::Shl_D1_ext4,
        /* 0x5 */ &Processor::Shr_D1_ext5,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::Sar_D1_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_D2(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::InstNotAvailable,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::Shl_D2_ext4,
        /* 0x5 */ &Processor::Shr_D2_ext5,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::Sar_D2_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_D3(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Rol_D3_ext0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::Rcl_D3_ext2,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::Shl_D3_ext4,
        /* 0x5 */ &Processor::Shr_D3_ext5,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::Sar_D3_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

// void Processor::Ext_D9(const Instruction *inst)
// {
//     static InstHandler handlers[] = {
//         /* 0x0 */ &Processor::InstNotAvailable,
//         /* 0x1 */ &Processor::InstNotAvailable,
//         /* 0x2 */ &Processor::InstNotAvailable,
//         /* 0x3 */ &Processor::InstNotAvailable,
//         /* 0x4 */ &Processor::InstNotAvailable,
//         /* 0x5 */ &Processor::InstNotAvailable,
//         /* 0x6 */ &Processor::InstNotAvailable,
//         /* 0x7 */ &Processor::Fstcw_D9_ext7,
//     };
//     return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
// }

void Processor::Ext_F6(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Test_F6_ext0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::Not_F6_ext2,
        /* 0x3 */ &Processor::Neg_F6_ext3,
        /* 0x4 */ &Processor::Mul_F6_ext4,
        /* 0x5 */ &Processor::Imul_F6_ext5,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_F7(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Test_F7_ext0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::Not_F7_ext2,
        /* 0x3 */ &Processor::Neg_F7_ext3,
        /* 0x4 */ &Processor::Mul_F7_ext4,
        /* 0x5 */ &Processor::Imul_F7_ext5,
        /* 0x6 */ &Processor::Div_F7_ext6,
        /* 0x7 */ &Processor::Idiv_F7_ext7,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_FE(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Inc_FE_ext0,
        /* 0x1 */ &Processor::Dec_FE_ext1,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::InstNotAvailable,
        /* 0x5 */ &Processor::InstNotAvailable,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_FF(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Inc_FF_ext0,
        /* 0x1 */ &Processor::Dec_FF_ext1,
        /* 0x2 */ &Processor::Call_FF_ext2,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::Jmp_FF_ext4,
        /* 0x5 */ &Processor::InstNotAvailable,
        /* 0x6 */ &Processor::Push_FF_ext6,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_0F1F(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::Nop_0F1F_0,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::InstNotAvailable,
        /* 0x5 */ &Processor::InstNotAvailable,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_0F72(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::InstNotAvailable,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::Psrld_0F72_ext2,
        /* 0x3 */ &Processor::InstNotAvailable,
        /* 0x4 */ &Processor::InstNotAvailable,
        /* 0x5 */ &Processor::InstNotAvailable,
        /* 0x6 */ &Processor::Pslld_0F72_ext6,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_0FAE(const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */ &Processor::InstNotAvailable,
        /* 0x1 */ &Processor::InstNotAvailable,
        /* 0x2 */ &Processor::InstNotAvailable,
        /* 0x3 */ &Processor::Stmxcsr_0FAE_3,
        /* 0x4 */ &Processor::InstNotAvailable,
        /* 0x5 */ &Processor::InstNotAvailable,
        /* 0x6 */ &Processor::InstNotAvailable,
        /* 0x7 */ &Processor::InstNotAvailable,
    };
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

void Processor::Ext_0FBA(const Instruction *inst)
{
	static InstHandler handlers[] = {
		/* 0x0 */ &Processor::InstNotAvailable,
		/* 0x1 */ &Processor::InstNotAvailable,
		/* 0x2 */ &Processor::InstNotAvailable,
		/* 0x3 */ &Processor::InstNotAvailable,
		/* 0x4 */ &Processor::Bt_0FBA_4,
		/* 0x5 */ &Processor::InstNotAvailable,
		/* 0x6 */ &Processor::InstNotAvailable,
		/* 0x7 */ &Processor::InstNotAvailable,
	};
    return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(inst);
}

END_NAMESPACE_LOCHSEMU()