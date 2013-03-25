#include "stdafx.h"
#include "processor.h"
#include "inst_table.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Ext_C6(Processor *cpu, const Instruction *inst)
{
    static InstHandler Ext_C6_Handlers[] = {
        /* 0x0 */Mov_C6_ext0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */InstNotAvailable,
        /* 0x5 */InstNotAvailable,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */InstNotAvailable,
    };
    return Ext_C6_Handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_C7(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Mov_C7_ext0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */InstNotAvailable,
        /* 0x5 */InstNotAvailable,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */InstNotAvailable,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_80(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Add_80_ext0,
        /* 0x1 */Or_80_ext1,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */And_80_ext4,
        /* 0x5 */Sub_80_ext5,
        /* 0x6 */Xor_80_ext6,
        /* 0x7 */Cmp_80_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_81(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Add_81_ext0,
        /* 0x1 */Or_81_ext1,
        /* 0x2 */Adc_81_ex2,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */And_81_ext4,
        /* 0x5 */Sub_81_ext5,
        /* 0x6 */Xor_81_ext6,
        /* 0x7 */Cmp_81_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_83(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Add_83_ext0,
        /* 0x1 */Or_83_ext1,
        /* 0x2 */Adc_83_ext2,
        /* 0x3 */Sbb_83_ext3,
        /* 0x4 */And_83_ext4,
        /* 0x5 */Sub_83_ext5,
        /* 0x6 */Xor_83_ext6,
        /* 0x7 */Cmp_83_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_8F(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Pop_8F_ext0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */InstNotAvailable,
        /* 0x5 */InstNotAvailable,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */InstNotAvailable,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_C0(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Rol_C0_ext0,
        /* 0x1 */Ror_C0_ext1,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */Shl_C0_ext4,
        /* 0x5 */Shr_C0_ext5,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */Sar_C0_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_C1(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Rol_C1_ext0,
        /* 0x1 */Ror_C1_ext1,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */Shl_C1_ext4,
        /* 0x5 */Shr_C1_ext5,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */Sar_C1_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_D0(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Rol_D0_ext0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */Rcl_D0_ext2,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */Shl_D0_ext4,
        /* 0x5 */Shr_D0_ext5,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */Sar_D0_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_D1(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Rol_D1_ext0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */Rcl_D1_ext2,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */Shl_D1_ext4,
        /* 0x5 */Shr_D1_ext5,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */Sar_D1_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_D2(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */InstNotAvailable,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */Shl_D2_ext4,
        /* 0x5 */Shr_D2_ext5,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */Sar_D2_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_D3(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Rol_D3_ext0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */Rcl_D3_ext2,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */Shl_D3_ext4,
        /* 0x5 */Shr_D3_ext5,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */Sar_D3_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

// LxResult Ext_D9(Processor *cpu, const Instruction *inst)
// {
//     static InstHandler handlers[] = {
//         /* 0x0 */InstNotAvailable,
//         /* 0x1 */InstNotAvailable,
//         /* 0x2 */InstNotAvailable,
//         /* 0x3 */InstNotAvailable,
//         /* 0x4 */InstNotAvailable,
//         /* 0x5 */InstNotAvailable,
//         /* 0x6 */InstNotAvailable,
//         /* 0x7 */Fstcw_D9_ext7,
//     };
//     return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
// }

LxResult Ext_F6(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Test_F6_ext0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */Not_F6_ext2,
        /* 0x3 */Neg_F6_ext3,
        /* 0x4 */Mul_F6_ext4,
        /* 0x5 */Imul_F6_ext5,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */InstNotAvailable,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_F7(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Test_F7_ext0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */Not_F7_ext2,
        /* 0x3 */Neg_F7_ext3,
        /* 0x4 */Mul_F7_ext4,
        /* 0x5 */Imul_F7_ext5,
        /* 0x6 */Div_F7_ext6,
        /* 0x7 */Idiv_F7_ext7,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_FE(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Inc_FE_ext0,
        /* 0x1 */Dec_FE_ext1,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */InstNotAvailable,
        /* 0x5 */InstNotAvailable,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */InstNotAvailable,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_FF(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Inc_FF_ext0,
        /* 0x1 */Dec_FF_ext1,
        /* 0x2 */Call_FF_ext2,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */Jmp_FF_ext4,
        /* 0x5 */InstNotAvailable,
        /* 0x6 */Push_FF_ext6,
        /* 0x7 */InstNotAvailable,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_0F1F(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */Nop_0F1F_0,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */InstNotAvailable,
        /* 0x5 */InstNotAvailable,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */InstNotAvailable,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_0F72(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */InstNotAvailable,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */Psrld_0F72_ext2,
        /* 0x3 */InstNotAvailable,
        /* 0x4 */InstNotAvailable,
        /* 0x5 */InstNotAvailable,
        /* 0x6 */Pslld_0F72_ext6,
        /* 0x7 */InstNotAvailable,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_0FAE(Processor *cpu, const Instruction *inst)
{
    static InstHandler handlers[] = {
        /* 0x0 */InstNotAvailable,
        /* 0x1 */InstNotAvailable,
        /* 0x2 */InstNotAvailable,
        /* 0x3 */Stmxcsr_0FAE_3,
        /* 0x4 */InstNotAvailable,
        /* 0x5 */InstNotAvailable,
        /* 0x6 */InstNotAvailable,
        /* 0x7 */InstNotAvailable,
    };
    return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

LxResult Ext_0FBA(Processor *cpu, const Instruction *inst)
{
	static InstHandler handlers[] = {
		/* 0x0 */InstNotAvailable,
		/* 0x1 */InstNotAvailable,
		/* 0x2 */InstNotAvailable,
		/* 0x3 */InstNotAvailable,
		/* 0x4 */Bt_0FBA_4,
		/* 0x5 */InstNotAvailable,
		/* 0x6 */InstNotAvailable,
		/* 0x7 */InstNotAvailable,
	};
	 return handlers[MASK_MODRM_REG(inst->Aux.modrm)](cpu, inst);
}

END_NAMESPACE_LOCHSEMU()