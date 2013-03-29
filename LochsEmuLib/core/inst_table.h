#pragma once

#ifndef __CORE_INST_TABLE_H__
#define __CORE_INST_TABLE_H__

#include "lochsemu.h"

#define DECLARE_INST_HANDLER(x)     LxResult    x(Processor *, const Instruction *inst)

BEGIN_NAMESPACE_LOCHSEMU()

extern InstHandler InstTableOneByte[];
extern InstHandler InstTableTwoBytes[];

/************************************************************************/
/* Instruction Handlers 1-byte                                          */
/************************************************************************/
DECLARE_INST_HANDLER(InstNotAvailable);
DECLARE_INST_HANDLER(Add_00);
DECLARE_INST_HANDLER(Add_01);
DECLARE_INST_HANDLER(Add_02);            // ADD r8, r/m8
DECLARE_INST_HANDLER(Add_03);            // ADD r16/32, r/m16/32
DECLARE_INST_HANDLER(Add_04);            // ADD Al, imm8
DECLARE_INST_HANDLER(Add_05);            // ADD EAX, imm32

DECLARE_INST_HANDLER(Or_08);
DECLARE_INST_HANDLER(Or_09);
DECLARE_INST_HANDLER(Or_0A);
DECLARE_INST_HANDLER(Or_0B);
DECLARE_INST_HANDLER(Or_0C);
DECLARE_INST_HANDLER(Or_0D);    

DECLARE_INST_HANDLER(Adc_12);           // ADC r8, r/m8
DECLARE_INST_HANDLER(Adc_13);           // ADC r32, r/m32

DECLARE_INST_HANDLER(Sbb_19);			// SBB r/m16/32, r/16/32
DECLARE_INST_HANDLER(Sbb_1A);           // SBB r8, r/m8
DECLARE_INST_HANDLER(Sbb_1B);            // SBB r16/32, r/m16/32

DECLARE_INST_HANDLER(And_20);            // AND r/m8, r8
DECLARE_INST_HANDLER(And_21);            // AND r/m16/32, r16/32
DECLARE_INST_HANDLER(And_22);            // AND r8, r/m8
DECLARE_INST_HANDLER(And_23);            // AND r16/32, r/m16/32
DECLARE_INST_HANDLER(And_24);            // AND AL, imm8
DECLARE_INST_HANDLER(And_25);            // AND EAX, imm32

DECLARE_INST_HANDLER(Sub_28);            // SUB r/m8, r8
DECLARE_INST_HANDLER(Sub_29);            // SUB r/m16/32, r16/32
DECLARE_INST_HANDLER(Sub_2A);            // SUB r8, r/m8
DECLARE_INST_HANDLER(Sub_2B);            // SUB r16/32, r/m16/32
DECLARE_INST_HANDLER(Sub_2C);            // SUB AL, imm8
DECLARE_INST_HANDLER(Sub_2D);            // SUB AX/EAX, imm16/32

DECLARE_INST_HANDLER(Xor_30);
DECLARE_INST_HANDLER(Xor_31);
DECLARE_INST_HANDLER(Xor_32);
DECLARE_INST_HANDLER(Xor_33);
DECLARE_INST_HANDLER(Xor_34);           // XOR AL, imm8
DECLARE_INST_HANDLER(Xor_35);           // XOR EAX, imm32

DECLARE_INST_HANDLER(Cmp_38);            // CMP r/m8, r8
DECLARE_INST_HANDLER(Cmp_39);            // CMP r/m16/32, r16/32
DECLARE_INST_HANDLER(Cmp_3A);            // CMP r8, r/m8
DECLARE_INST_HANDLER(Cmp_3B);            // CMP r16/32, r/m16/32
DECLARE_INST_HANDLER(Cmp_3C);            // CMP AL, imm8
DECLARE_INST_HANDLER(Cmp_3D);            // CMP AX/EAX, imm16/32

DECLARE_INST_HANDLER(Inc_40X);           // INC r16/32
DECLARE_INST_HANDLER(Dec_48X);           // DEC r32
DECLARE_INST_HANDLER(Push_50X);
DECLARE_INST_HANDLER(Pop_58X);
DECLARE_INST_HANDLER(Pushad_60);         //pushad
DECLARE_INST_HANDLER(Popad_61);          //popad
DECLARE_INST_HANDLER(Bound_62);         // bound r32, m32&32
DECLARE_INST_HANDLER(Push_68);
DECLARE_INST_HANDLER(Imul_69);          // IMUL r32, r/m32, imm32
DECLARE_INST_HANDLER(Push_6A);
DECLARE_INST_HANDLER(Imul_6B);          // IMUL r32, r/m32, imm8

DECLARE_INST_HANDLER(Jo_70);            // OF=1,        JO rel8
DECLARE_INST_HANDLER(Jno_71);           // OF=0,        JNO rel8
DECLARE_INST_HANDLER(Jb_72);            // CF=1,        JB rel8, JNAE rel8
DECLARE_INST_HANDLER(Jnb_73);           // CF=0,        JNB rel8, JNC rel8
DECLARE_INST_HANDLER(Je_74);            // Zf=1,        JE rel8, JZ rel8
DECLARE_INST_HANDLER(Jne_75);           // ZF=0,        JNE rel8, JNZ rel8
DECLARE_INST_HANDLER(Jbe_76);           // CF=1|ZF=1,   JBE rel8, JNA rel8
DECLARE_INST_HANDLER(Ja_77);            // CF=0&ZF=0,   JA rel8, JNBE rel8
DECLARE_INST_HANDLER(Js_78);            // SF=1,        JS rel8
DECLARE_INST_HANDLER(Jns_79);           // SF=0,        JNS rel8
DECLARE_INST_HANDLER(Jp_7A);            // PF=1,        JP rel8, JPE rel8
DECLARE_INST_HANDLER(Jnp_7B);           // PF=0,        JNP rel8, JPO rel8
DECLARE_INST_HANDLER(Jl_7C);            // SF!=OF,      JL rel8, JNGE rel8
DECLARE_INST_HANDLER(Jge_7D);           // SF=OF,       JGE rel8, JNL rel8
DECLARE_INST_HANDLER(Jle_7E);           // ZF=1|SF!=OF, JLE rel8, JNG rel8
DECLARE_INST_HANDLER(Jg_7F);            // ZF=0&SF=OF,  JG rel8, JNLE rel8

DECLARE_INST_HANDLER(Ext_80);
DECLARE_INST_HANDLER(Add_80_ext0);       // ADD r/m8, imm8
DECLARE_INST_HANDLER(Or_80_ext1);        // OR r/m8, imm8
DECLARE_INST_HANDLER(Adc_81_ex2);		// ADC r/m16/32, imm16/32
DECLARE_INST_HANDLER(And_80_ext4);       // AND r/m8, imm8
DECLARE_INST_HANDLER(Sub_80_ext5);       // SUB r/m8, imm8
DECLARE_INST_HANDLER(Xor_80_ext6);       //Xor r/m8, imm8
DECLARE_INST_HANDLER(Cmp_80_ext7);       // CMP r/m8, imm8

DECLARE_INST_HANDLER(Ext_81);           
DECLARE_INST_HANDLER(Add_81_ext0);       // ADD r/m16/32, imm16/32
DECLARE_INST_HANDLER(Or_81_ext1);        // OR r/m32, imm32
DECLARE_INST_HANDLER(And_81_ext4);       // AND r/m16/32, imm16/32
DECLARE_INST_HANDLER(Sub_81_ext5);       // SUB r/m16/32, imm16/32
DECLARE_INST_HANDLER(Xor_81_ext6);       // Xor r/m16/32, imm16/32
DECLARE_INST_HANDLER(Cmp_81_ext7);       // CMP r/m16/32, imm16/32

DECLARE_INST_HANDLER(Ext_83);
DECLARE_INST_HANDLER(Add_83_ext0);       // ADD r/m16/32, imm8
DECLARE_INST_HANDLER(Or_83_ext1);        // OR r/m32, imm8
DECLARE_INST_HANDLER(Adc_83_ext2);		// ADC r/m16/32, imm8
DECLARE_INST_HANDLER(Sbb_83_ext3);       // SBB r/m16/32, imm8
DECLARE_INST_HANDLER(And_83_ext4);       // AND r/m16/32, imm8
DECLARE_INST_HANDLER(Sub_83_ext5);       // SUB r/m16/32, imm8
DECLARE_INST_HANDLER(Xor_83_ext6);       // XOR r/m32, imm8
DECLARE_INST_HANDLER(Cmp_83_ext7);       // CMP r/m16/32, imm8
DECLARE_INST_HANDLER(Xlat_D7);			 //	XLAT m8

DECLARE_INST_HANDLER(Test_84);           // TEST r/m8, r8
DECLARE_INST_HANDLER(Test_85);           // TEST r/m16/32, r16/32

DECLARE_INST_HANDLER(Xchg_87);          // XCHG r/m32, r32; XCHG r32, r/m32

DECLARE_INST_HANDLER(Mov_88);
DECLARE_INST_HANDLER(Mov_89);
DECLARE_INST_HANDLER(Mov_8A);
DECLARE_INST_HANDLER(Mov_8B);
DECLARE_INST_HANDLER(Mov_8C);           // MOV r/m16, sreg

DECLARE_INST_HANDLER(Lea_8D);

DECLARE_INST_HANDLER(Ext_8F);
DECLARE_INST_HANDLER(Pop_8F_ext0);
DECLARE_INST_HANDLER(Nop_90);           // NOP
DECLARE_INST_HANDLER(Xchg_90X);         // XCHG EAX, r32
DECLARE_INST_HANDLER(Cbw_98);
DECLARE_INST_HANDLER(Cdq_99);           // CDQ

DECLARE_INST_HANDLER(Pushf_9C);         // PUSHF
DECLARE_INST_HANDLER(Popf_9D);          // POPF
DECLARE_INST_HANDLER(Sahf_9E);          // SAHF

DECLARE_INST_HANDLER(Mov_A0);            // MOV AL, moffs8
DECLARE_INST_HANDLER(Mov_A1);            // MOV AX/EAX, moffs16/32
DECLARE_INST_HANDLER(Mov_A2);            // MOV moffs8, AL
DECLARE_INST_HANDLER(Mov_A3);            // MOV moffs16/32, AX/EAX

DECLARE_INST_HANDLER(Movsb_A4);         // MOVSB
DECLARE_INST_HANDLER(Movs_A5);          // REP MOVSD
DECLARE_INST_HANDLER(Cmpsb_A6);			// CMPSB
DECLARE_INST_HANDLER(Cmpsd_A7);         // CMPSW/CMPSD
DECLARE_INST_HANDLER(Test_A8);          // TEST AL, imm8
DECLARE_INST_HANDLER(Test_A9);          // TEST EAX, imm32
DECLARE_INST_HANDLER(Stos_AA);          // STOS m8
DECLARE_INST_HANDLER(Stos_AB);          // STOS m32
DECLARE_INST_HANDLER(Lodsb_AC);         // LODSB
DECLARE_INST_HANDLER(Lodsd_AD);         // LODSD
DECLARE_INST_HANDLER(Scasb_AE);         // SCASB

DECLARE_INST_HANDLER(Mov_B0X);           // MOV r8, imm8
DECLARE_INST_HANDLER(Mov_B8X);           // MOV r16/32, imm16/32

DECLARE_INST_HANDLER(Jecxz_E3);         // JECXZ rel8

DECLARE_INST_HANDLER(Ext_C0);
DECLARE_INST_HANDLER(Rol_C0_ext0);      // ROL r/m8, imm8
DECLARE_INST_HANDLER(Ror_C0_ext1);      // ROR r/m8, imm8
DECLARE_INST_HANDLER(Shl_C0_ext4);      // SHL r/m8, imm8
DECLARE_INST_HANDLER(Shr_C0_ext5);       // SHR r/m8, imm8
DECLARE_INST_HANDLER(Sar_C0_ext7);      // SAR r/m8, imm8
DECLARE_INST_HANDLER(Ext_C1);
DECLARE_INST_HANDLER(Ror_C1_ext1);      // ROR r/m32, imm8
DECLARE_INST_HANDLER(Rol_C1_ext0);      // ROL r/m32, imm8
DECLARE_INST_HANDLER(Shl_C1_ext4);      // SHL r/m16/32, imm8
DECLARE_INST_HANDLER(Shr_C1_ext5);       // SHR r/m16/32, imm8
DECLARE_INST_HANDLER(Sar_C1_ext7);      // SAR r/m8, imm8

DECLARE_INST_HANDLER(Retn_C2);           // RETN imm16
DECLARE_INST_HANDLER(Ret_C3);            // RET

DECLARE_INST_HANDLER(Ext_C6);
DECLARE_INST_HANDLER(Mov_C6_ext0);

DECLARE_INST_HANDLER(Ext_C7);
DECLARE_INST_HANDLER(Mov_C7_ext0);
DECLARE_INST_HANDLER(Enter_C8);
DECLARE_INST_HANDLER(Leave_C9);

DECLARE_INST_HANDLER(Int3_CC);

DECLARE_INST_HANDLER(Ext_D0);
DECLARE_INST_HANDLER(Rol_D0_ext0);      // ROL r/m8, 1
DECLARE_INST_HANDLER(Rcl_D0_ext2);      // RCL r/m8, 1
DECLARE_INST_HANDLER(Shl_D0_ext4);       // SHL r/m8, 1
DECLARE_INST_HANDLER(Shr_D0_ext5);       // SHR r/m8, 1
DECLARE_INST_HANDLER(Sar_D0_ext7);      // SAR r/m8, 1

DECLARE_INST_HANDLER(Ext_D1);
DECLARE_INST_HANDLER(Rol_D1_ext0);      // ROL r/m32, 1
DECLARE_INST_HANDLER(Rcl_D1_ext2);		// RCL r/m16, 1
DECLARE_INST_HANDLER(Shl_D1_ext4);      // SHL r/m16/32, 1
DECLARE_INST_HANDLER(Shr_D1_ext5);       // SHR r/m32, 1
DECLARE_INST_HANDLER(Sar_D1_ext7);      // SAR r/m16/32, 1

DECLARE_INST_HANDLER(Ext_D2);
DECLARE_INST_HANDLER(Shl_D2_ext4);      // SHL r/m8, CL
DECLARE_INST_HANDLER(Shr_D2_ext5);       // SHR r/m8, CL
DECLARE_INST_HANDLER(Sar_D2_ext7);      // SAR r/m8, CL

DECLARE_INST_HANDLER(Ext_D3);
DECLARE_INST_HANDLER(Rol_D3_ext0);      // ROL r/m32, CL
DECLARE_INST_HANDLER(Rcl_D3_ext2);      // RCL r/m32, CL
DECLARE_INST_HANDLER(Shl_D3_ext4);      // SHR r/m16/32, CL
DECLARE_INST_HANDLER(Shr_D3_ext5);       // SHR r/m32, CL
DECLARE_INST_HANDLER(Sar_D3_ext7);      // SAR r/m16/32, CL

DECLARE_INST_HANDLER(Fpu_Ext);       // FPU stuff, 

DECLARE_INST_HANDLER(Loop_E2);			// LOOP rel8
DECLARE_INST_HANDLER(Jecxz_E3);         // JECXZ rel8
DECLARE_INST_HANDLER(Call_E8);
DECLARE_INST_HANDLER(Jmp_E9);           // JMP rel32
DECLARE_INST_HANDLER(Jmp_EB);

DECLARE_INST_HANDLER(Cmc_F5);			//CMC
DECLARE_INST_HANDLER(Ext_F6);
DECLARE_INST_HANDLER(Test_F6_ext0);      // TEST r/m8, imm8
DECLARE_INST_HANDLER(Not_F6_ext2);       // NOT r/m8
DECLARE_INST_HANDLER(Neg_F6_ext3);       // NEG r/m8
DECLARE_INST_HANDLER(Mul_F6_ext4);       // MUL r/m8
DECLARE_INST_HANDLER(Imul_F6_ext5);      // IMUL r/m8

DECLARE_INST_HANDLER(Ext_F7);
DECLARE_INST_HANDLER(Test_F7_ext0);      // TEST r/m16/32, imm16/32
DECLARE_INST_HANDLER(Not_F7_ext2);       // NOT r/m16/32
DECLARE_INST_HANDLER(Neg_F7_ext3);       // NEG r/m16/32
DECLARE_INST_HANDLER(Mul_F7_ext4);       // MUL r/m16/32
DECLARE_INST_HANDLER(Imul_F7_ext5);     // IMUL r/m32
DECLARE_INST_HANDLER(Div_F7_ext6);      // DIV r/m32
DECLARE_INST_HANDLER(Idiv_F7_ext7);     // IDIV r/m32
DECLARE_INST_HANDLER(Clc_F8);			//CLC
DECLARE_INST_HANDLER(Stc_F9);			//STC
DECLARE_INST_HANDLER(Cli_FA);           // CLI
DECLARE_INST_HANDLER(Sti_FB);           // STI
DECLARE_INST_HANDLER(Cld_FC);           // CLD
DECLARE_INST_HANDLER(Std_FD);           // STD

DECLARE_INST_HANDLER(Ext_FE);
DECLARE_INST_HANDLER(Dec_FE_ext1);
DECLARE_INST_HANDLER(Inc_FE_ext0);       // INC r/m8

DECLARE_INST_HANDLER(Ext_FF);
DECLARE_INST_HANDLER(Inc_FF_ext0);       // INC r/m16/32
DECLARE_INST_HANDLER(Dec_FF_ext1);       // DEC r/m32
DECLARE_INST_HANDLER(Call_FF_ext2);      // CALL r/m16/32
DECLARE_INST_HANDLER(Jmp_FF_ext4);
DECLARE_INST_HANDLER(Push_FF_ext6);

/************************************************************************/
/* Instruction Handlers 2-byte                                          */
/************************************************************************/
// DECLARE_INST_HANDLER(Fldcs_0F6D);       // FLDCS
// DECLARE_INST_HANDLER(Fstcw_0F7D);       // FSTCW

DECLARE_INST_HANDLER(Xgetbv_0F01);          // XGETBV
DECLARE_INST_HANDLER(Ext_0F1F);             // 
DECLARE_INST_HANDLER(Nop_0F1F_0);           // NOP

DECLARE_INST_HANDLER(Rdtsc_0F31);           // RDTSC

DECLARE_INST_HANDLER(Movapd_66_0F28);       // MOVAPD

DECLARE_INST_HANDLER(Cmovo_0F40);            // OF=1,        CmovO rel8
DECLARE_INST_HANDLER(Cmovno_0F41);           // OF=0,        CmovNO rel8
DECLARE_INST_HANDLER(Cmovb_0F42);            // CF=1,        CmovB rel8, CmovNAE rel8
DECLARE_INST_HANDLER(Cmovnb_0F43);           // CF=0,        CmovNB rel8, CmovNC rel8
DECLARE_INST_HANDLER(Cmove_0F44);            // Zf=1,        CmovE rel8, CmovZ rel8
DECLARE_INST_HANDLER(Cmovne_0F45);           // ZF=0,        CmovNE rel8, CmovNZ rel8
DECLARE_INST_HANDLER(Cmovbe_0F46);           // CF=1|ZF=1,   CmovBE rel8, CmovNA rel8
DECLARE_INST_HANDLER(Cmova_0F47);            // CF=0&ZF=0,   CmovA rel8, CmovNBE rel8
DECLARE_INST_HANDLER(Cmovs_0F48);            // SF=1,        CmovS rel8
DECLARE_INST_HANDLER(Cmovns_0F49);           // SF=0,        CmovNS rel8
DECLARE_INST_HANDLER(Cmovp_0F4A);            // PF=1,        CmovP rel8, CmovPE rel8
DECLARE_INST_HANDLER(Cmovnp_0F4B);           // PF=0,        CmovNP rel8, CmovPO rel8
DECLARE_INST_HANDLER(Cmovl_0F4C);            // SF!=OF,      CmovL rel8, CmovNGE rel8
DECLARE_INST_HANDLER(Cmovge_0F4D);           // SF=OF,       CmovGE rel8, CmovNL rel8
DECLARE_INST_HANDLER(Cmovle_0F4E);           // ZF=1|SF!=OF, CmovLE rel8, CmovNG rel8
DECLARE_INST_HANDLER(Cmovg_0F4F);            // ZF=0&SF=OF,  CmovG rel8, CmovNLE rel8

DECLARE_INST_HANDLER(Xorps_0F57);           // XORPS
DECLARE_INST_HANDLER(Punpckldq_0F62);       // PUNPCKLDQ
DECLARE_INST_HANDLER(Pcmpgtb_0F64);         // PCMPGTB
DECLARE_INST_HANDLER(Movd_0F6E);            // MOVD
DECLARE_INST_HANDLER(Movdqa_0F6F);          // MOVDQA
DECLARE_INST_HANDLER(Pshufw_0F70);          // PSHUFW
DECLARE_INST_HANDLER(Ext_0F72); 
DECLARE_INST_HANDLER(Psrld_0F72_ext2);      // PSRLD xmm1, imm8
DECLARE_INST_HANDLER(Pslld_0F72_ext6);      // PSLLD xmm1, imm8
DECLARE_INST_HANDLER(Emms_0F77);            // EMMS
DECLARE_INST_HANDLER(Movd_0F7E);            // MOVD
DECLARE_INST_HANDLER(Movdqa_0F7F);          // MOVDQA

DECLARE_INST_HANDLER(Jo_0F80);          // OF=1, JO rel32
DECLARE_INST_HANDLER(Jno_0F81);         // OF=0, JNO rel32
DECLARE_INST_HANDLER(Jc_0F82);          // CF=1, JC rel32
DECLARE_INST_HANDLER(Jnb_0F83);         // CF=0, JNB rel32
DECLARE_INST_HANDLER(Jz_0F84);          // ZF=1, JZ rel16/32
DECLARE_INST_HANDLER(Jnz_0F85);         // ZF=0, JNZ rel16/32
DECLARE_INST_HANDLER(Jna_0F86);         // CF=1 | ZF=1 JNA
DECLARE_INST_HANDLER(Ja_0F87);          // CF = 0, ZF = 0, JA rel32
DECLARE_INST_HANDLER(Js_0F88);          // SF=1, JS rel32
DECLARE_INST_HANDLER(Jns_0F89);         // SF=0, JNS rel32
DECLARE_INST_HANDLER(Jp_0F8A);          // PF=1, JP rel32
DECLARE_INST_HANDLER(Jpo_0F8B);         // PF=0, JPO rel32
DECLARE_INST_HANDLER(Jl_0F8C);          // SF != OF
DECLARE_INST_HANDLER(Jnl_0F8D);         // SF=OF
DECLARE_INST_HANDLER(Jng_0F8E);         // ZF=1 | SF != OF, JNG rel32
DECLARE_INST_HANDLER(Jnle_0F8F);        // ZF=0 & SF=OF, JNLE rel32

DECLARE_INST_HANDLER(Seto_0F90);          // OF=1, JO rel32
DECLARE_INST_HANDLER(Setno_0F91);         // OF=0, JNO rel32
DECLARE_INST_HANDLER(Setc_0F92);          // CF=1, JC rel32
DECLARE_INST_HANDLER(Setnb_0F93);         // CF=0, JNB rel32
DECLARE_INST_HANDLER(Setz_0F94);          // ZF=1, JZ rel16/32
DECLARE_INST_HANDLER(Setnz_0F95);         // ZF=0, JNZ rel16/32
DECLARE_INST_HANDLER(Setna_0F96);         // CF=1 | ZF=1 JNA
DECLARE_INST_HANDLER(Seta_0F97);          // CF = 0, ZF = 0, JA rel32
DECLARE_INST_HANDLER(Sets_0F98);          // SF=1, JS rel32
DECLARE_INST_HANDLER(Setns_0F99);         // SF=0, JNS rel32
DECLARE_INST_HANDLER(Setp_0F9A);          // PF=1, JP rel32
DECLARE_INST_HANDLER(Setpo_0F9B);         // PF=0, JPO rel32
DECLARE_INST_HANDLER(Setl_0F9C);          // SF != OF
DECLARE_INST_HANDLER(Setnl_0F9D);         // SF=OF
DECLARE_INST_HANDLER(Setng_0F9E);         // ZF=1 | SF != OF, JNG rel32
DECLARE_INST_HANDLER(Setnle_0F9F);        // ZF=0 & SF=OF, JNLE rel32
// 
// DECLARE_INST_HANDLER(Setz_0F94);        // SETZ r/m8
// DECLARE_INST_HANDLER(Setne_0F95);         // SETNE r/m8
// DECLARE_INST_HANDLER(Setnbe_0F97);		// SETNBE r/m8
// DECLARE_INST_HANDLER(Setnl_0F9D);       // SETNL r/m8
// DECLARE_INST_HANDLER(Setle_0F9E);       // SETLE r/m8
// DECLARE_INST_HANDLER(Setnle_0F9F);      // SETNLE r/m8

DECLARE_INST_HANDLER(Cpuid_0FA2);       
DECLARE_INST_HANDLER(Bt_0FA3);			// BT r/m16/32, r16/32
DECLARE_INST_HANDLER(Shld_0FA4);        // SHLD r/m32, r32, imm8
DECLARE_INST_HANDLER(Shld_0FA5);        // SHLD r/m32, r32, CL
DECLARE_INST_HANDLER(Shrd_0FAC);        // SHRD r/m32, r32, imm8
DECLARE_INST_HANDLER(Shrd_0FAD);        // SHRD r/m32, r32, CL

DECLARE_INST_HANDLER(Ext_0FAE);         
DECLARE_INST_HANDLER(Stmxcsr_0FAE_3);   // STMXCSR

DECLARE_INST_HANDLER(Imul_0FAF);        // IMUL r32, r/m32
DECLARE_INST_HANDLER(Cmpxchg_0FB1);     // CMPXCHG r/m32, r32
DECLARE_INST_HANDLER(Movzx_0FB6);         // MOVZX r16/32, r/m8
DECLARE_INST_HANDLER(Movzx_0FB7);         // MOVZX r32, r/m16

DECLARE_INST_HANDLER(Ext_0FBA);
DECLARE_INST_HANDLER(Bt_0FBA_4);		// BT r/m16/32, imm8

DECLARE_INST_HANDLER(Movsx_0FBE);       // MOVSX  r32, r/m8
DECLARE_INST_HANDLER(Movsx_0FBF);       // MOVSX  r32, r/m16

DECLARE_INST_HANDLER(Xadd_0FC1);		//XADD r/m16/32, r16/32
DECLARE_INST_HANDLER(Bswap_0FC8);		//BSWAP r32;

DECLARE_INST_HANDLER(Movq_0FD6);        // MOVQ xmm2/m64, xmm1
DECLARE_INST_HANDLER(Pand_0FDB);        // PAND xmm1, xmm2/m128

DECLARE_INST_HANDLER(Pxor_0FEF);		//PXOR xmm1,xmm2/m128
DECLARE_INST_HANDLER(Paddb_0FFC);       // PADDB xmm1, xmm2/m128

/************************************************************************/
/* Instruction Handlers FPU                                             */
/************************************************************************/

DECLARE_INST_HANDLER(Fpu_Wait_9B);          // WAIT
DECLARE_INST_HANDLER(Fpu_Fadd32fp_D8_0);	// FADD m32fp
DECLARE_INST_HANDLER(Fpu_Fdiv32fp_D8_6);    // FDIV m32fp
DECLARE_INST_HANDLER(Fpu_Fadd_D8C0);        // FADD ST(0), ST(i)
DECLARE_INST_HANDLER(Fpu_Fmul_D8C8);        // FMUL ST(0), ST(i)
DECLARE_INST_HANDLER(Fpu_Fcom_D8D0);        // FCOM ST(0), ST(i)
DECLARE_INST_HANDLER(Fpu_Fsub_D8E0);        // FSUB ST(0), ST(i)
DECLARE_INST_HANDLER(Fpu_Fsubr_D8E8);       // FSUBR ST(0), ST(i)
DECLARE_INST_HANDLER(Fpu_Fdiv_D8F0);        // FDIV ST(0), st(i)
DECLARE_INST_HANDLER(Fpu_Fldcw_D9_5);       // FLDCW
DECLARE_INST_HANDLER(Fpu_Fstcw_D9_7);       // FSTCW
DECLARE_INST_HANDLER(Fpu_Fld_D9C0);         // FLD
DECLARE_INST_HANDLER(Fpu_Fxch_D9C8);        // FXCH
DECLARE_INST_HANDLER(Fpu_Fchs_D9E0);        // FCHS
DECLARE_INST_HANDLER(Fpu_Fabs_D9E1);        // FABS
DECLARE_INST_HANDLER(Fpu_Fxam_D9E5);        // FXAM
DECLARE_INST_HANDLER(Fpu_Fld1_D9E8);        // FLD1
DECLARE_INST_HANDLER(Fpu_Fldl2t_D9E9);      // FLDL2T
DECLARE_INST_HANDLER(Fpu_Fldl2e_D9EA);      // FLDL2E
DECLARE_INST_HANDLER(Fpu_Fldpi_D9EB);       // FLDPI
DECLARE_INST_HANDLER(Fpu_Fldlg2_D9EC);      // FLDLG2
DECLARE_INST_HANDLER(Fpu_Fldln2_D9ED);      // FLDLN2
DECLARE_INST_HANDLER(Fpu_Fldz_D9EE);        // FLDZ
DECLARE_INST_HANDLER(Fpu_Fyl2x_D9F1);       // FYL2X
DECLARE_INST_HANDLER(Fpu_Fptan_D9F2);       // FPTAN
DECLARE_INST_HANDLER(Fpu_Fprem1_D9F5);      // FPREM1
DECLARE_INST_HANDLER(Fpu_Fprem_D9F8);       // FPREM
DECLARE_INST_HANDLER(Fpu_Fsqrt_D9FA);       // FSQRT
DECLARE_INST_HANDLER(Fpu_Frndint_D9FC);     // FRNDINT
DECLARE_INST_HANDLER(Fpu_Fidiv_DA_6);       // FIDIV m32int
DECLARE_INST_HANDLER(Fpu_Fucompp_DAE9);		// FUCOMPP
DECLARE_INST_HANDLER(Fpu_Fild32_DB_0);      // FILD
DECLARE_INST_HANDLER(Fpu_Fistp32_DB_3);		// FISTP m32int
DECLARE_INST_HANDLER(Fpu_Fld80_DB_5);       // FLD m80fp
DECLARE_INST_HANDLER(Fpu_Fnclex_DBE2);      // FNCLEX
DECLARE_INST_HANDLER(Fpu_Fninit_DBE3);      // FNINIT
DECLARE_INST_HANDLER(Fpu_Fadd_DC_0);        // FADD m64fp
DECLARE_INST_HANDLER(Fpu_Fmul_DC_1);        // FMUL m64fp
DECLARE_INST_HANDLER(Fpu_Fcom_DC_2);        // FCOM m64fp
DECLARE_INST_HANDLER(Fpu_Fcomp_DC_3);       // FCOMP m64fp
DECLARE_INST_HANDLER(Fpu_Fsub_DC_4);        // FSUB m64fp
DECLARE_INST_HANDLER(Fpu_Fsubr_DC_5);       // FSUBR m64fp
DECLARE_INST_HANDLER(Fpu_Fdiv_DC_6);        // FDIV m64fp
DECLARE_INST_HANDLER(Fpu_Fdivr_DC_7);       // FDIVR m64fp
DECLARE_INST_HANDLER(Fpu_Fadd_DCC0);        // FADD ST(i), ST(0)
DECLARE_INST_HANDLER(Fpu_Fmul_DCC8);        // FMUL ST(i), ST(0)
DECLARE_INST_HANDLER(Fpu_Fsubr_DCE0);       // FSUBR ST(i), ST(0)
DECLARE_INST_HANDLER(Fpu_Fdivr_DCF0);       // FDIVR ST(i), ST(0)
DECLARE_INST_HANDLER(Fpu_Fdiv_DCF8);        // FDIV st(i), st(0)
DECLARE_INST_HANDLER(Fpu_Fld64fp_DD_0);     // FLD m64fp
DECLARE_INST_HANDLER(Fpu_Fst64fp_DD_2);     // FST m64fp
DECLARE_INST_HANDLER(Fpu_Fstp64fp_DD_3);    // FSTP m64fp
DECLARE_INST_HANDLER(Fpu_Fstsw_DD_7);       // FSTSW m2byte
DECLARE_INST_HANDLER(Fpu_Fstp_DDD8);        // FSTP ST(i)
DECLARE_INST_HANDLER(Fpu_Fidiv_DE_6);       // FIDIV m64int
DECLARE_INST_HANDLER(Fpu_Faddp_DEC0);       // FADDP ST(i), ST(0)
DECLARE_INST_HANDLER(Fpu_Fmulp_DEC8);       // FMULP st(i), st(0)
DECLARE_INST_HANDLER(Fpu_Fsubrp_DEE0);      // FSUBRP ST(i), ST(0)
DECLARE_INST_HANDLER(Fpu_Fsubp_DEE8);       // FSUBP st(i), st(0)
DECLARE_INST_HANDLER(Fpu_Fdivp_DEF8);       // FDIVP st(i), st(0)
DECLARE_INST_HANDLER(Fpu_Fild16_DF_0);		// FILD m16int
DECLARE_INST_HANDLER(Fpu_Fild64_DF_5);		// FILD m64int
DECLARE_INST_HANDLER(Fpu_Fistp64int_DF_7);  // FISTP m64int
DECLARE_INST_HANDLER(Fpu_Fstsw_DFE0);       // FSTSW AX
END_NAMESPACE_LOCHSEMU()

#endif // __CORE_INST_TABLE_H__
