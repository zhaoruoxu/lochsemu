#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

Processor::InstHandler Processor::Fpu_Ext_D8_Handlers[] = 
{
    /* 0x00 */ &Processor::Fpu_Fadd32fp_D8_0,
    /* 0x08 */ &Processor::InstNotAvailable,
    /* 0x10 */ &Processor::InstNotAvailable,
    /* 0x18 */ &Processor::InstNotAvailable,
    /* 0x20 */ &Processor::InstNotAvailable,
    /* 0x28 */ &Processor::InstNotAvailable,
    /* 0x30 */ &Processor::Fpu_Fdiv32fp_D8_6,
    /* 0x38 */ &Processor::InstNotAvailable,

    /* 0xC0 */ &Processor::Fpu_Fadd_D8C0,
    /* 0xC1 */ &Processor::Fpu_Fadd_D8C0,
    /* 0xC2 */ &Processor::Fpu_Fadd_D8C0,
    /* 0xC3 */ &Processor::Fpu_Fadd_D8C0,
    /* 0xC4 */ &Processor::Fpu_Fadd_D8C0,
    /* 0xC5 */ &Processor::Fpu_Fadd_D8C0,
    /* 0xC6 */ &Processor::Fpu_Fadd_D8C0,
    /* 0xC7 */ &Processor::Fpu_Fadd_D8C0,
    /* 0xC8 */ &Processor::Fpu_Fmul_D8C8,
    /* 0xC9 */ &Processor::Fpu_Fmul_D8C8,
    /* 0xCA */ &Processor::Fpu_Fmul_D8C8,
    /* 0xCB */ &Processor::Fpu_Fmul_D8C8,
    /* 0xCC */ &Processor::Fpu_Fmul_D8C8,
    /* 0xCD */ &Processor::Fpu_Fmul_D8C8,
    /* 0xCE */ &Processor::Fpu_Fmul_D8C8,
    /* 0xCF */ &Processor::Fpu_Fmul_D8C8,
    /* 0xD0 */ &Processor::Fpu_Fcom_D8D0,
    /* 0xD1 */ &Processor::Fpu_Fcom_D8D0,
    /* 0xD2 */ &Processor::Fpu_Fcom_D8D0,
    /* 0xD3 */ &Processor::Fpu_Fcom_D8D0,
    /* 0xD4 */ &Processor::Fpu_Fcom_D8D0,
    /* 0xD5 */ &Processor::Fpu_Fcom_D8D0,
    /* 0xD6 */ &Processor::Fpu_Fcom_D8D0,
    /* 0xD7 */ &Processor::Fpu_Fcom_D8D0,
    /* 0xD8 */ &Processor::InstNotAvailable,
    /* 0xD9 */ &Processor::InstNotAvailable,
    /* 0xDA */ &Processor::InstNotAvailable,
    /* 0xDB */ &Processor::InstNotAvailable,
    /* 0xDC */ &Processor::InstNotAvailable,
    /* 0xDD */ &Processor::InstNotAvailable,
    /* 0xDE */ &Processor::InstNotAvailable,
    /* 0xDF */ &Processor::InstNotAvailable,
    /* 0xE0 */ &Processor::Fpu_Fsub_D8E0,
    /* 0xE1 */ &Processor::Fpu_Fsub_D8E0,
    /* 0xE2 */ &Processor::Fpu_Fsub_D8E0,
    /* 0xE3 */ &Processor::Fpu_Fsub_D8E0,
    /* 0xE4 */ &Processor::Fpu_Fsub_D8E0,
    /* 0xE5 */ &Processor::Fpu_Fsub_D8E0,
    /* 0xE6 */ &Processor::Fpu_Fsub_D8E0,
    /* 0xE7 */ &Processor::Fpu_Fsub_D8E0,
    /* 0xE8 */ &Processor::Fpu_Fsubr_D8E8,
    /* 0xE9 */ &Processor::Fpu_Fsubr_D8E8,
    /* 0xEA */ &Processor::Fpu_Fsubr_D8E8,
    /* 0xEB */ &Processor::Fpu_Fsubr_D8E8,
    /* 0xEC */ &Processor::Fpu_Fsubr_D8E8,
    /* 0xED */ &Processor::Fpu_Fsubr_D8E8,
    /* 0xEE */ &Processor::Fpu_Fsubr_D8E8,
    /* 0xEF */ &Processor::Fpu_Fsubr_D8E8,
    /* 0xF0 */ &Processor::Fpu_Fdiv_D8F0,
    /* 0xF1 */ &Processor::Fpu_Fdiv_D8F0,
    /* 0xF2 */ &Processor::Fpu_Fdiv_D8F0,
    /* 0xF3 */ &Processor::Fpu_Fdiv_D8F0,
    /* 0xF4 */ &Processor::Fpu_Fdiv_D8F0,
    /* 0xF5 */ &Processor::Fpu_Fdiv_D8F0,
    /* 0xF6 */ &Processor::Fpu_Fdiv_D8F0,
    /* 0xF7 */ &Processor::Fpu_Fdiv_D8F0,
    /* 0xF8 */ &Processor::InstNotAvailable,
    /* 0xF9 */ &Processor::InstNotAvailable,
    /* 0xFA */ &Processor::InstNotAvailable,
    /* 0xFB */ &Processor::InstNotAvailable,
    /* 0xFC */ &Processor::InstNotAvailable,
    /* 0xFD */ &Processor::InstNotAvailable,
    /* 0xFE */ &Processor::InstNotAvailable,
    /* 0xFF */ &Processor::InstNotAvailable,
};

Processor::InstHandler Processor::Fpu_Ext_D9_Handlers[] = 
{
    /* 0x00 */ &Processor::InstNotAvailable,
    /* 0x08 */ &Processor::InstNotAvailable,
    /* 0x10 */ &Processor::InstNotAvailable,
    /* 0x18 */ &Processor::InstNotAvailable,
    /* 0x20 */ &Processor::InstNotAvailable,
    /* 0x28 */ &Processor::Fpu_Fldcw_D9_5,
    /* 0x30 */ &Processor::InstNotAvailable,
    /* 0x38 */ &Processor::Fpu_Fstcw_D9_7,

    /* 0xC0 */ &Processor::Fpu_Fld_D9C0,
    /* 0xC1 */ &Processor::Fpu_Fld_D9C0,
    /* 0xC2 */ &Processor::Fpu_Fld_D9C0,
    /* 0xC3 */ &Processor::Fpu_Fld_D9C0,
    /* 0xC4 */ &Processor::Fpu_Fld_D9C0,
    /* 0xC5 */ &Processor::Fpu_Fld_D9C0,
    /* 0xC6 */ &Processor::Fpu_Fld_D9C0,
    /* 0xC7 */ &Processor::Fpu_Fld_D9C0,
    /* 0xC8 */ &Processor::Fpu_Fxch_D9C8,
    /* 0xC9 */ &Processor::Fpu_Fxch_D9C8,
    /* 0xCA */ &Processor::Fpu_Fxch_D9C8,
    /* 0xCB */ &Processor::Fpu_Fxch_D9C8,
    /* 0xCC */ &Processor::Fpu_Fxch_D9C8,
    /* 0xCD */ &Processor::Fpu_Fxch_D9C8,
    /* 0xCE */ &Processor::Fpu_Fxch_D9C8,
    /* 0xCF */ &Processor::Fpu_Fxch_D9C8,
    /* 0xD0 */ &Processor::InstNotAvailable,
    /* 0xD1 */ &Processor::InstNotAvailable,
    /* 0xD2 */ &Processor::InstNotAvailable,
    /* 0xD3 */ &Processor::InstNotAvailable,
    /* 0xD4 */ &Processor::InstNotAvailable,
    /* 0xD5 */ &Processor::InstNotAvailable,
    /* 0xD6 */ &Processor::InstNotAvailable,
    /* 0xD7 */ &Processor::InstNotAvailable,
    /* 0xD8 */ &Processor::InstNotAvailable,
    /* 0xD9 */ &Processor::InstNotAvailable,
    /* 0xDA */ &Processor::InstNotAvailable,
    /* 0xDB */ &Processor::InstNotAvailable,
    /* 0xDC */ &Processor::InstNotAvailable,
    /* 0xDD */ &Processor::InstNotAvailable,
    /* 0xDE */ &Processor::InstNotAvailable,
    /* 0xDF */ &Processor::InstNotAvailable,
    /* 0xE0 */ &Processor::Fpu_Fchs_D9E0,
    /* 0xE1 */ &Processor::Fpu_Fabs_D9E1,
    /* 0xE2 */ &Processor::InstNotAvailable,
    /* 0xE3 */ &Processor::InstNotAvailable,
    /* 0xE4 */ &Processor::InstNotAvailable,
    /* 0xE5 */ &Processor::Fpu_Fxam_D9E5,
    /* 0xE6 */ &Processor::InstNotAvailable,
    /* 0xE7 */ &Processor::InstNotAvailable,
    /* 0xE8 */ &Processor::Fpu_Fld1_D9E8,
    /* 0xE9 */ &Processor::Fpu_Fldl2t_D9E9,
    /* 0xEA */ &Processor::Fpu_Fldl2e_D9EA,
    /* 0xEB */ &Processor::Fpu_Fldpi_D9EB,
    /* 0xEC */ &Processor::Fpu_Fldlg2_D9EC,
    /* 0xED */ &Processor::Fpu_Fldln2_D9ED,
    /* 0xEE */ &Processor::Fpu_Fldz_D9EE,
    /* 0xEF */ &Processor::InstNotAvailable,
    /* 0xF0 */ &Processor::InstNotAvailable,
    /* 0xF1 */ &Processor::Fpu_Fyl2x_D9F1,
    /* 0xF2 */ &Processor::Fpu_Fptan_D9F2,
    /* 0xF3 */ &Processor::InstNotAvailable,
    /* 0xF4 */ &Processor::InstNotAvailable,
    /* 0xF5 */ &Processor::Fpu_Fprem1_D9F5,
    /* 0xF6 */ &Processor::InstNotAvailable,
    /* 0xF7 */ &Processor::InstNotAvailable,
    /* 0xF8 */ &Processor::Fpu_Fprem_D9F8,
    /* 0xF9 */ &Processor::InstNotAvailable,
    /* 0xFA */ &Processor::Fpu_Fsqrt_D9FA,
    /* 0xFB */ &Processor::InstNotAvailable,
    /* 0xFC */ &Processor::Fpu_Frndint_D9FC,
    /* 0xFD */ &Processor::InstNotAvailable,
    /* 0xFE */ &Processor::InstNotAvailable,
    /* 0xFF */ &Processor::InstNotAvailable,
};

Processor::InstHandler Processor::Fpu_Ext_DA_Handlers[] = 
{
    /* 0x00 */ &Processor::InstNotAvailable,
    /* 0x08 */ &Processor::InstNotAvailable,
    /* 0x10 */ &Processor::InstNotAvailable,
    /* 0x18 */ &Processor::InstNotAvailable,
    /* 0x20 */ &Processor::InstNotAvailable,
    /* 0x28 */ &Processor::InstNotAvailable,
    /* 0x30 */ &Processor::Fpu_Fidiv_DA_6,
    /* 0x38 */ &Processor::InstNotAvailable,

    /* 0xC0 */ &Processor::InstNotAvailable,
    /* 0xC1 */ &Processor::InstNotAvailable,
    /* 0xC2 */ &Processor::InstNotAvailable,
    /* 0xC3 */ &Processor::InstNotAvailable,
    /* 0xC4 */ &Processor::InstNotAvailable,
    /* 0xC5 */ &Processor::InstNotAvailable,
    /* 0xC6 */ &Processor::InstNotAvailable,
    /* 0xC7 */ &Processor::InstNotAvailable,
    /* 0xC8 */ &Processor::InstNotAvailable,
    /* 0xC9 */ &Processor::InstNotAvailable,
    /* 0xCA */ &Processor::InstNotAvailable,
    /* 0xCB */ &Processor::InstNotAvailable,
    /* 0xCC */ &Processor::InstNotAvailable,
    /* 0xCD */ &Processor::InstNotAvailable,
    /* 0xCE */ &Processor::InstNotAvailable,
    /* 0xCF */ &Processor::InstNotAvailable,
    /* 0xD0 */ &Processor::InstNotAvailable,
    /* 0xD1 */ &Processor::InstNotAvailable,
    /* 0xD2 */ &Processor::InstNotAvailable,
    /* 0xD3 */ &Processor::InstNotAvailable,
    /* 0xD4 */ &Processor::InstNotAvailable,
    /* 0xD5 */ &Processor::InstNotAvailable,
    /* 0xD6 */ &Processor::InstNotAvailable,
    /* 0xD7 */ &Processor::InstNotAvailable,
    /* 0xD8 */ &Processor::InstNotAvailable,
    /* 0xD9 */ &Processor::InstNotAvailable,
    /* 0xDA */ &Processor::InstNotAvailable,
    /* 0xDB */ &Processor::InstNotAvailable,
    /* 0xDC */ &Processor::InstNotAvailable,
    /* 0xDD */ &Processor::InstNotAvailable,
    /* 0xDE */ &Processor::InstNotAvailable,
    /* 0xDF */ &Processor::InstNotAvailable,
    /* 0xE0 */ &Processor::InstNotAvailable,
    /* 0xE1 */ &Processor::InstNotAvailable,
    /* 0xE2 */ &Processor::InstNotAvailable,
    /* 0xE3 */ &Processor::InstNotAvailable,
    /* 0xE4 */ &Processor::InstNotAvailable,
    /* 0xE5 */ &Processor::InstNotAvailable,
    /* 0xE6 */ &Processor::InstNotAvailable,
    /* 0xE7 */ &Processor::InstNotAvailable,
    /* 0xE8 */ &Processor::InstNotAvailable,
    /* 0xE9 */ &Processor::Fpu_Fucompp_DAE9,
    /* 0xEA */ &Processor::InstNotAvailable,
    /* 0xEB */ &Processor::InstNotAvailable,
    /* 0xEC */ &Processor::InstNotAvailable,
    /* 0xED */ &Processor::InstNotAvailable,
    /* 0xEE */ &Processor::InstNotAvailable,
    /* 0xEF */ &Processor::InstNotAvailable,
    /* 0xF0 */ &Processor::InstNotAvailable,
    /* 0xF1 */ &Processor::InstNotAvailable,
    /* 0xF2 */ &Processor::InstNotAvailable,
    /* 0xF3 */ &Processor::InstNotAvailable,
    /* 0xF4 */ &Processor::InstNotAvailable,
    /* 0xF5 */ &Processor::InstNotAvailable,
    /* 0xF6 */ &Processor::InstNotAvailable,
    /* 0xF7 */ &Processor::InstNotAvailable,
    /* 0xF8 */ &Processor::InstNotAvailable,
    /* 0xF9 */ &Processor::InstNotAvailable,
    /* 0xFA */ &Processor::InstNotAvailable,
    /* 0xFB */ &Processor::InstNotAvailable,
    /* 0xFC */ &Processor::InstNotAvailable,
    /* 0xFD */ &Processor::InstNotAvailable,
    /* 0xFE */ &Processor::InstNotAvailable,
    /* 0xFF */ &Processor::InstNotAvailable,
};

Processor::InstHandler Processor::Fpu_Ext_DB_Handlers[] = 
{
    /* 0x00 */ &Processor::Fpu_Fild32_DB_0,
    /* 0x08 */ &Processor::InstNotAvailable,
    /* 0x10 */ &Processor::InstNotAvailable,
    /* 0x18 */ &Processor::Fpu_Fistp32_DB_3,
    /* 0x20 */ &Processor::InstNotAvailable,
    /* 0x28 */ &Processor::Fpu_Fld80_DB_5,
    /* 0x30 */ &Processor::InstNotAvailable,
    /* 0x38 */ &Processor::InstNotAvailable,

    /* 0xC0 */ &Processor::InstNotAvailable,
    /* 0xC1 */ &Processor::InstNotAvailable,
    /* 0xC2 */ &Processor::InstNotAvailable,
    /* 0xC3 */ &Processor::InstNotAvailable,
    /* 0xC4 */ &Processor::InstNotAvailable,
    /* 0xC5 */ &Processor::InstNotAvailable,
    /* 0xC6 */ &Processor::InstNotAvailable,
    /* 0xC7 */ &Processor::InstNotAvailable,
    /* 0xC8 */ &Processor::InstNotAvailable,
    /* 0xC9 */ &Processor::InstNotAvailable,
    /* 0xCA */ &Processor::InstNotAvailable,
    /* 0xCB */ &Processor::InstNotAvailable,
    /* 0xCC */ &Processor::InstNotAvailable,
    /* 0xCD */ &Processor::InstNotAvailable,
    /* 0xCE */ &Processor::InstNotAvailable,
    /* 0xCF */ &Processor::InstNotAvailable,
    /* 0xD0 */ &Processor::InstNotAvailable,
    /* 0xD1 */ &Processor::InstNotAvailable,
    /* 0xD2 */ &Processor::InstNotAvailable,
    /* 0xD3 */ &Processor::InstNotAvailable,
    /* 0xD4 */ &Processor::InstNotAvailable,
    /* 0xD5 */ &Processor::InstNotAvailable,
    /* 0xD6 */ &Processor::InstNotAvailable,
    /* 0xD7 */ &Processor::InstNotAvailable,
    /* 0xD8 */ &Processor::InstNotAvailable,
    /* 0xD9 */ &Processor::InstNotAvailable,
    /* 0xDA */ &Processor::InstNotAvailable,
    /* 0xDB */ &Processor::InstNotAvailable,
    /* 0xDC */ &Processor::InstNotAvailable,
    /* 0xDD */ &Processor::InstNotAvailable,
    /* 0xDE */ &Processor::InstNotAvailable,
    /* 0xDF */ &Processor::InstNotAvailable,
    /* 0xE0 */ &Processor::InstNotAvailable,
    /* 0xE1 */ &Processor::InstNotAvailable,
    /* 0xE2 */ &Processor::Fpu_Fnclex_DBE2,
    /* 0xE3 */ &Processor::Fpu_Fninit_DBE3,
    /* 0xE4 */ &Processor::InstNotAvailable,
    /* 0xE5 */ &Processor::InstNotAvailable,
    /* 0xE6 */ &Processor::InstNotAvailable,
    /* 0xE7 */ &Processor::InstNotAvailable,
    /* 0xE8 */ &Processor::InstNotAvailable,
    /* 0xE9 */ &Processor::InstNotAvailable,
    /* 0xEA */ &Processor::InstNotAvailable,
    /* 0xEB */ &Processor::InstNotAvailable,
    /* 0xEC */ &Processor::InstNotAvailable,
    /* 0xED */ &Processor::InstNotAvailable,
    /* 0xEE */ &Processor::InstNotAvailable,
    /* 0xEF */ &Processor::InstNotAvailable,
    /* 0xF0 */ &Processor::InstNotAvailable,
    /* 0xF1 */ &Processor::InstNotAvailable,
    /* 0xF2 */ &Processor::InstNotAvailable,
    /* 0xF3 */ &Processor::InstNotAvailable,
    /* 0xF4 */ &Processor::InstNotAvailable,
    /* 0xF5 */ &Processor::InstNotAvailable,
    /* 0xF6 */ &Processor::InstNotAvailable,
    /* 0xF7 */ &Processor::InstNotAvailable,
    /* 0xF8 */ &Processor::InstNotAvailable,
    /* 0xF9 */ &Processor::InstNotAvailable,
    /* 0xFA */ &Processor::InstNotAvailable,
    /* 0xFB */ &Processor::InstNotAvailable,
    /* 0xFC */ &Processor::InstNotAvailable,
    /* 0xFD */ &Processor::InstNotAvailable,
    /* 0xFE */ &Processor::InstNotAvailable,
    /* 0xFF */ &Processor::InstNotAvailable,
};

Processor::InstHandler Processor::Fpu_Ext_DC_Handlers[] = 
{
    /* 0x00 */ &Processor::Fpu_Fadd_DC_0,
    /* 0x08 */ &Processor::Fpu_Fmul_DC_1,
    /* 0x10 */ &Processor::Fpu_Fcom_DC_2,
    /* 0x18 */ &Processor::Fpu_Fcomp_DC_3,
    /* 0x20 */ &Processor::Fpu_Fsub_DC_4,
    /* 0x28 */ &Processor::Fpu_Fsubr_DC_5,
    /* 0x30 */ &Processor::Fpu_Fdiv_DC_6,
    /* 0x38 */ &Processor::Fpu_Fdivr_DC_7,

    /* 0xC0 */ &Processor::Fpu_Fadd_DCC0,
    /* 0xC1 */ &Processor::Fpu_Fadd_DCC0,
    /* 0xC2 */ &Processor::Fpu_Fadd_DCC0,
    /* 0xC3 */ &Processor::Fpu_Fadd_DCC0,
    /* 0xC4 */ &Processor::Fpu_Fadd_DCC0,
    /* 0xC5 */ &Processor::Fpu_Fadd_DCC0,
    /* 0xC6 */ &Processor::Fpu_Fadd_DCC0,
    /* 0xC7 */ &Processor::Fpu_Fadd_DCC0,
    /* 0xC8 */ &Processor::Fpu_Fmul_DCC8,
    /* 0xC9 */ &Processor::Fpu_Fmul_DCC8,
    /* 0xCA */ &Processor::Fpu_Fmul_DCC8,
    /* 0xCB */ &Processor::Fpu_Fmul_DCC8,
    /* 0xCC */ &Processor::Fpu_Fmul_DCC8,
    /* 0xCD */ &Processor::Fpu_Fmul_DCC8,
    /* 0xCE */ &Processor::Fpu_Fmul_DCC8,
    /* 0xCF */ &Processor::Fpu_Fmul_DCC8,
    /* 0xD0 */ &Processor::InstNotAvailable,
    /* 0xD1 */ &Processor::InstNotAvailable,
    /* 0xD2 */ &Processor::InstNotAvailable,
    /* 0xD3 */ &Processor::InstNotAvailable,
    /* 0xD4 */ &Processor::InstNotAvailable,
    /* 0xD5 */ &Processor::InstNotAvailable,
    /* 0xD6 */ &Processor::InstNotAvailable,
    /* 0xD7 */ &Processor::InstNotAvailable,
    /* 0xD8 */ &Processor::InstNotAvailable,
    /* 0xD9 */ &Processor::InstNotAvailable,
    /* 0xDA */ &Processor::InstNotAvailable,
    /* 0xDB */ &Processor::InstNotAvailable,
    /* 0xDC */ &Processor::InstNotAvailable,
    /* 0xDD */ &Processor::InstNotAvailable,
    /* 0xDE */ &Processor::InstNotAvailable,
    /* 0xDF */ &Processor::InstNotAvailable,
    /* 0xE0 */ &Processor::Fpu_Fsubr_DCE0,
    /* 0xE1 */ &Processor::Fpu_Fsubr_DCE0,
    /* 0xE2 */ &Processor::Fpu_Fsubr_DCE0,
    /* 0xE3 */ &Processor::Fpu_Fsubr_DCE0,
    /* 0xE4 */ &Processor::Fpu_Fsubr_DCE0,
    /* 0xE5 */ &Processor::Fpu_Fsubr_DCE0,
    /* 0xE6 */ &Processor::Fpu_Fsubr_DCE0,
    /* 0xE7 */ &Processor::Fpu_Fsubr_DCE0,
    /* 0xE8 */ &Processor::InstNotAvailable,
    /* 0xE9 */ &Processor::InstNotAvailable,
    /* 0xEA */ &Processor::InstNotAvailable,
    /* 0xEB */ &Processor::InstNotAvailable,
    /* 0xEC */ &Processor::InstNotAvailable,
    /* 0xED */ &Processor::InstNotAvailable,
    /* 0xEE */ &Processor::InstNotAvailable,
    /* 0xEF */ &Processor::InstNotAvailable,
    /* 0xF0 */ &Processor::Fpu_Fdivr_DCF0,
    /* 0xF1 */ &Processor::Fpu_Fdivr_DCF0,
    /* 0xF2 */ &Processor::Fpu_Fdivr_DCF0,
    /* 0xF3 */ &Processor::Fpu_Fdivr_DCF0,
    /* 0xF4 */ &Processor::Fpu_Fdivr_DCF0,
    /* 0xF5 */ &Processor::Fpu_Fdivr_DCF0,
    /* 0xF6 */ &Processor::Fpu_Fdivr_DCF0,
    /* 0xF7 */ &Processor::Fpu_Fdivr_DCF0,
    /* 0xF8 */ &Processor::Fpu_Fdiv_DCF8,
    /* 0xF9 */ &Processor::Fpu_Fdiv_DCF8,
    /* 0xFA */ &Processor::Fpu_Fdiv_DCF8,
    /* 0xFB */ &Processor::Fpu_Fdiv_DCF8,
    /* 0xFC */ &Processor::Fpu_Fdiv_DCF8,
    /* 0xFD */ &Processor::Fpu_Fdiv_DCF8,
    /* 0xFE */ &Processor::Fpu_Fdiv_DCF8,
    /* 0xFF */ &Processor::Fpu_Fdiv_DCF8,
};

Processor::InstHandler Processor::Fpu_Ext_DD_Handlers[] = 
{
    /* 0x00 */ &Processor::Fpu_Fld64fp_DD_0,
    /* 0x08 */ &Processor::InstNotAvailable,
    /* 0x10 */ &Processor::Fpu_Fst64fp_DD_2,
    /* 0x18 */ &Processor::Fpu_Fstp64fp_DD_3,
    /* 0x20 */ &Processor::InstNotAvailable,
    /* 0x28 */ &Processor::InstNotAvailable,
    /* 0x30 */ &Processor::InstNotAvailable,
    /* 0x38 */ &Processor::Fpu_Fstsw_DD_7,

    /* 0xC0 */ &Processor::InstNotAvailable,
    /* 0xC1 */ &Processor::InstNotAvailable,
    /* 0xC2 */ &Processor::InstNotAvailable,
    /* 0xC3 */ &Processor::InstNotAvailable,
    /* 0xC4 */ &Processor::InstNotAvailable,
    /* 0xC5 */ &Processor::InstNotAvailable,
    /* 0xC6 */ &Processor::InstNotAvailable,
    /* 0xC7 */ &Processor::InstNotAvailable,
    /* 0xC8 */ &Processor::InstNotAvailable,
    /* 0xC9 */ &Processor::InstNotAvailable,
    /* 0xCA */ &Processor::InstNotAvailable,
    /* 0xCB */ &Processor::InstNotAvailable,
    /* 0xCC */ &Processor::InstNotAvailable,
    /* 0xCD */ &Processor::InstNotAvailable,
    /* 0xCE */ &Processor::InstNotAvailable,
    /* 0xCF */ &Processor::InstNotAvailable,
    /* 0xD0 */ &Processor::InstNotAvailable,
    /* 0xD1 */ &Processor::InstNotAvailable,
    /* 0xD2 */ &Processor::InstNotAvailable,
    /* 0xD3 */ &Processor::InstNotAvailable,
    /* 0xD4 */ &Processor::InstNotAvailable,
    /* 0xD5 */ &Processor::InstNotAvailable,
    /* 0xD6 */ &Processor::InstNotAvailable,
    /* 0xD7 */ &Processor::InstNotAvailable,
    /* 0xD8 */ &Processor::Fpu_Fstp_DDD8,
    /* 0xD9 */ &Processor::Fpu_Fstp_DDD8,
    /* 0xDA */ &Processor::Fpu_Fstp_DDD8,
    /* 0xDB */ &Processor::Fpu_Fstp_DDD8,
    /* 0xDC */ &Processor::Fpu_Fstp_DDD8,
    /* 0xDD */ &Processor::Fpu_Fstp_DDD8,
    /* 0xDE */ &Processor::Fpu_Fstp_DDD8,
    /* 0xDF */ &Processor::Fpu_Fstp_DDD8,
    /* 0xE0 */ &Processor::InstNotAvailable,
    /* 0xE1 */ &Processor::InstNotAvailable,
    /* 0xE2 */ &Processor::InstNotAvailable,
    /* 0xE3 */ &Processor::InstNotAvailable,
    /* 0xE4 */ &Processor::InstNotAvailable,
    /* 0xE5 */ &Processor::InstNotAvailable,
    /* 0xE6 */ &Processor::InstNotAvailable,
    /* 0xE7 */ &Processor::InstNotAvailable,
    /* 0xE8 */ &Processor::InstNotAvailable,
    /* 0xE9 */ &Processor::InstNotAvailable,
    /* 0xEA */ &Processor::InstNotAvailable,
    /* 0xEB */ &Processor::InstNotAvailable,
    /* 0xEC */ &Processor::InstNotAvailable,
    /* 0xED */ &Processor::InstNotAvailable,
    /* 0xEE */ &Processor::InstNotAvailable,
    /* 0xEF */ &Processor::InstNotAvailable,
    /* 0xF0 */ &Processor::InstNotAvailable,
    /* 0xF1 */ &Processor::InstNotAvailable,
    /* 0xF2 */ &Processor::InstNotAvailable,
    /* 0xF3 */ &Processor::InstNotAvailable,
    /* 0xF4 */ &Processor::InstNotAvailable,
    /* 0xF5 */ &Processor::InstNotAvailable,
    /* 0xF6 */ &Processor::InstNotAvailable,
    /* 0xF7 */ &Processor::InstNotAvailable,
    /* 0xF8 */ &Processor::InstNotAvailable,
    /* 0xF9 */ &Processor::InstNotAvailable,
    /* 0xFA */ &Processor::InstNotAvailable,
    /* 0xFB */ &Processor::InstNotAvailable,
    /* 0xFC */ &Processor::InstNotAvailable,
    /* 0xFD */ &Processor::InstNotAvailable,
    /* 0xFE */ &Processor::InstNotAvailable,
    /* 0xFF */ &Processor::InstNotAvailable,
};

Processor::InstHandler Processor::Fpu_Ext_DE_Handlers[] = 
{
    /* 0x00 */ &Processor::InstNotAvailable,
    /* 0x08 */ &Processor::InstNotAvailable,
    /* 0x10 */ &Processor::InstNotAvailable,
    /* 0x18 */ &Processor::InstNotAvailable,
    /* 0x20 */ &Processor::InstNotAvailable,
    /* 0x28 */ &Processor::InstNotAvailable,
    /* 0x30 */ &Processor::Fpu_Fidiv_DE_6,
    /* 0x38 */ &Processor::InstNotAvailable,

    /* 0xC0 */ &Processor::Fpu_Faddp_DEC0,
    /* 0xC1 */ &Processor::Fpu_Faddp_DEC0,
    /* 0xC2 */ &Processor::Fpu_Faddp_DEC0,
    /* 0xC3 */ &Processor::Fpu_Faddp_DEC0,
    /* 0xC4 */ &Processor::Fpu_Faddp_DEC0,
    /* 0xC5 */ &Processor::Fpu_Faddp_DEC0,
    /* 0xC6 */ &Processor::Fpu_Faddp_DEC0,
    /* 0xC7 */ &Processor::Fpu_Faddp_DEC0,
    /* 0xC8 */ &Processor::Fpu_Fmulp_DEC8,
    /* 0xC9 */ &Processor::Fpu_Fmulp_DEC8,
    /* 0xCA */ &Processor::Fpu_Fmulp_DEC8,
    /* 0xCB */ &Processor::Fpu_Fmulp_DEC8,
    /* 0xCC */ &Processor::Fpu_Fmulp_DEC8,
    /* 0xCD */ &Processor::Fpu_Fmulp_DEC8,
    /* 0xCE */ &Processor::Fpu_Fmulp_DEC8,
    /* 0xCF */ &Processor::Fpu_Fmulp_DEC8,
    /* 0xD0 */ &Processor::InstNotAvailable,
    /* 0xD1 */ &Processor::InstNotAvailable,
    /* 0xD2 */ &Processor::InstNotAvailable,
    /* 0xD3 */ &Processor::InstNotAvailable,
    /* 0xD4 */ &Processor::InstNotAvailable,
    /* 0xD5 */ &Processor::InstNotAvailable,
    /* 0xD6 */ &Processor::InstNotAvailable,
    /* 0xD7 */ &Processor::InstNotAvailable,
    /* 0xD8 */ &Processor::InstNotAvailable,
    /* 0xD9 */ &Processor::InstNotAvailable,
    /* 0xDA */ &Processor::InstNotAvailable,
    /* 0xDB */ &Processor::InstNotAvailable,
    /* 0xDC */ &Processor::InstNotAvailable,
    /* 0xDD */ &Processor::InstNotAvailable,
    /* 0xDE */ &Processor::InstNotAvailable,
    /* 0xDF */ &Processor::InstNotAvailable,
    /* 0xE0 */ &Processor::Fpu_Fsubrp_DEE0,
    /* 0xE1 */ &Processor::Fpu_Fsubrp_DEE0,
    /* 0xE2 */ &Processor::Fpu_Fsubrp_DEE0,
    /* 0xE3 */ &Processor::Fpu_Fsubrp_DEE0,
    /* 0xE4 */ &Processor::Fpu_Fsubrp_DEE0,
    /* 0xE5 */ &Processor::Fpu_Fsubrp_DEE0,
    /* 0xE6 */ &Processor::Fpu_Fsubrp_DEE0,
    /* 0xE7 */ &Processor::Fpu_Fsubrp_DEE0,
    /* 0xE8 */ &Processor::Fpu_Fsubp_DEE8,
    /* 0xE9 */ &Processor::Fpu_Fsubp_DEE8,
    /* 0xEA */ &Processor::Fpu_Fsubp_DEE8,
    /* 0xEB */ &Processor::Fpu_Fsubp_DEE8,
    /* 0xEC */ &Processor::Fpu_Fsubp_DEE8,
    /* 0xED */ &Processor::Fpu_Fsubp_DEE8,
    /* 0xEE */ &Processor::Fpu_Fsubp_DEE8,
    /* 0xEF */ &Processor::Fpu_Fsubp_DEE8,
    /* 0xF0 */ &Processor::InstNotAvailable,
    /* 0xF1 */ &Processor::InstNotAvailable,
    /* 0xF2 */ &Processor::InstNotAvailable,
    /* 0xF3 */ &Processor::InstNotAvailable,
    /* 0xF4 */ &Processor::InstNotAvailable,
    /* 0xF5 */ &Processor::InstNotAvailable,
    /* 0xF6 */ &Processor::InstNotAvailable,
    /* 0xF7 */ &Processor::InstNotAvailable,
    /* 0xF8 */ &Processor::Fpu_Fdivp_DEF8,
    /* 0xF9 */ &Processor::Fpu_Fdivp_DEF8,
    /* 0xFA */ &Processor::Fpu_Fdivp_DEF8,
    /* 0xFB */ &Processor::Fpu_Fdivp_DEF8,
    /* 0xFC */ &Processor::Fpu_Fdivp_DEF8,
    /* 0xFD */ &Processor::Fpu_Fdivp_DEF8,
    /* 0xFE */ &Processor::Fpu_Fdivp_DEF8,
    /* 0xFF */ &Processor::Fpu_Fdivp_DEF8,
};

Processor::InstHandler Processor::Fpu_Ext_DF_Handlers[] = 
{
    /* 0x00 */ &Processor::Fpu_Fild16_DF_0,
    /* 0x08 */ &Processor::InstNotAvailable,
    /* 0x10 */ &Processor::InstNotAvailable,
    /* 0x18 */ &Processor::InstNotAvailable,
    /* 0x20 */ &Processor::InstNotAvailable,
    /* 0x28 */ &Processor::Fpu_Fild64_DF_5,
    /* 0x30 */ &Processor::InstNotAvailable,
    /* 0x38 */ &Processor::Fpu_Fistp64int_DF_7,

    /* 0xC0 */ &Processor::InstNotAvailable,
    /* 0xC1 */ &Processor::InstNotAvailable,
    /* 0xC2 */ &Processor::InstNotAvailable,
    /* 0xC3 */ &Processor::InstNotAvailable,
    /* 0xC4 */ &Processor::InstNotAvailable,
    /* 0xC5 */ &Processor::InstNotAvailable,
    /* 0xC6 */ &Processor::InstNotAvailable,
    /* 0xC7 */ &Processor::InstNotAvailable,
    /* 0xC8 */ &Processor::InstNotAvailable,
    /* 0xC9 */ &Processor::InstNotAvailable,
    /* 0xCA */ &Processor::InstNotAvailable,
    /* 0xCB */ &Processor::InstNotAvailable,
    /* 0xCC */ &Processor::InstNotAvailable,
    /* 0xCD */ &Processor::InstNotAvailable,
    /* 0xCE */ &Processor::InstNotAvailable,
    /* 0xCF */ &Processor::InstNotAvailable,
    /* 0xD0 */ &Processor::InstNotAvailable,
    /* 0xD1 */ &Processor::InstNotAvailable,
    /* 0xD2 */ &Processor::InstNotAvailable,
    /* 0xD3 */ &Processor::InstNotAvailable,
    /* 0xD4 */ &Processor::InstNotAvailable,
    /* 0xD5 */ &Processor::InstNotAvailable,
    /* 0xD6 */ &Processor::InstNotAvailable,
    /* 0xD7 */ &Processor::InstNotAvailable,
    /* 0xD8 */ &Processor::InstNotAvailable,
    /* 0xD9 */ &Processor::InstNotAvailable,
    /* 0xDA */ &Processor::InstNotAvailable,
    /* 0xDB */ &Processor::InstNotAvailable,
    /* 0xDC */ &Processor::InstNotAvailable,
    /* 0xDD */ &Processor::InstNotAvailable,
    /* 0xDE */ &Processor::InstNotAvailable,
    /* 0xDF */ &Processor::InstNotAvailable,
    /* 0xE0 */ &Processor::Fpu_Fstsw_DFE0,
    /* 0xE1 */ &Processor::InstNotAvailable,
    /* 0xE2 */ &Processor::InstNotAvailable,
    /* 0xE3 */ &Processor::InstNotAvailable,
    /* 0xE4 */ &Processor::InstNotAvailable,
    /* 0xE5 */ &Processor::InstNotAvailable,
    /* 0xE6 */ &Processor::InstNotAvailable,
    /* 0xE7 */ &Processor::InstNotAvailable,
    /* 0xE8 */ &Processor::InstNotAvailable,
    /* 0xE9 */ &Processor::InstNotAvailable,
    /* 0xEA */ &Processor::InstNotAvailable,
    /* 0xEB */ &Processor::InstNotAvailable,
    /* 0xEC */ &Processor::InstNotAvailable,
    /* 0xED */ &Processor::InstNotAvailable,
    /* 0xEE */ &Processor::InstNotAvailable,
    /* 0xEF */ &Processor::InstNotAvailable,
    /* 0xF0 */ &Processor::InstNotAvailable,
    /* 0xF1 */ &Processor::InstNotAvailable,
    /* 0xF2 */ &Processor::InstNotAvailable,
    /* 0xF3 */ &Processor::InstNotAvailable,
    /* 0xF4 */ &Processor::InstNotAvailable,
    /* 0xF5 */ &Processor::InstNotAvailable,
    /* 0xF6 */ &Processor::InstNotAvailable,
    /* 0xF7 */ &Processor::InstNotAvailable,
    /* 0xF8 */ &Processor::InstNotAvailable,
    /* 0xF9 */ &Processor::InstNotAvailable,
    /* 0xFA */ &Processor::InstNotAvailable,
    /* 0xFB */ &Processor::InstNotAvailable,
    /* 0xFC */ &Processor::InstNotAvailable,
    /* 0xFD */ &Processor::InstNotAvailable,
    /* 0xFE */ &Processor::InstNotAvailable,
    /* 0xFF */ &Processor::InstNotAvailable,
};

Processor::InstHandler *  Processor::Fpu_Handlers[] = 
{
    Fpu_Ext_D8_Handlers,
    Fpu_Ext_D9_Handlers,
    Fpu_Ext_DA_Handlers,
    Fpu_Ext_DB_Handlers,
    Fpu_Ext_DC_Handlers,
    Fpu_Ext_DD_Handlers,
    Fpu_Ext_DE_Handlers,
    Fpu_Ext_DF_Handlers,
};

void Processor::Fpu_Ext(const Instruction *inst)
{
    Assert(inst->Main.Inst.Opcode >= 0xd8 && inst->Main.Inst.Opcode <= 0xdf);
    int index = inst->Aux.opcode < 0xc0 ? MASK_MODRM_REG(inst->Aux.opcode) : inst->Aux.opcode - 0xb8;
    Assert(index >= 0 && index < 0x48);
    return (this->*(Fpu_Handlers[inst->Main.Inst.Opcode - 0xd8])[index])(inst);
}

END_NAMESPACE_LOCHSEMU()