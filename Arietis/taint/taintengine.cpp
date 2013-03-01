#include "stdafx.h"
#include "taintengine.h"
#include "processor.h"
#include "engine.h"




TaintEngine::TaintEngine(AEngine *engine)
    : m_engine(engine)
{
    m_archive = m_engine->GetArchive();
}

TaintEngine::~TaintEngine()
{

}

void TaintEngine::Initialize()
{

}


void TaintEngine::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    if (!IsEnabled()) return;

    TaintInstHandler h = NULL;
    if (INST_ONEBYTE(inst->Main.Inst.Opcode)) {
        h = HandlerOneByte[inst->Main.Inst.Opcode];
    } else if (INST_TWOBYTE(inst->Main.Inst.Opcode)) {
        h = HandlerTwoBytes[inst->Main.Inst.Opcode & 0xff];
    } else {
        Assert(0);
    }

    if (NULL != h) {
        (this->*h)(cpu, inst);
    }
}



void TaintEngine::UpdateInstContext( InstContext *ctx ) const
{
    for (int i = 0; i < InstContext::RegCount; i++)
        ctx->regTaint[i]    = CpuTaint.GPRegs[i];
    ctx->EipTaint           = CpuTaint.Eip;
    for (int i = 0; i < InstContext::FlagCount; i++)
        ctx->flagTaint[i]   = CpuTaint.Flags[i];
}

bool TaintEngine::IsEnabled() const
{
    return m_archive->IsTaintEnabled;
}

void TaintEngine::Enable( bool isEnabled )
{
    m_archive->IsTaintEnabled = isEnabled;
}


void TaintEngine::DebugTaintIntroduce(const Processor *cpu, const Instruction *inst)
{
    Taint4 t;
    t.SetAll();
    TaintPropagate(cpu, inst, t);
}

void TaintEngine::DefaultBinopHandler(const Processor *cpu, const Instruction *inst)
{
    Assert(ARG1.ArgSize == ARG2.ArgSize || IsConstantArg(ARG2));

    if (ARG1.ArgSize == 32) {
        TaintPropagate_Binop<4>(cpu, inst);
    } else if (ARG1.ArgSize == 8) {
        TaintPropagate_Binop<1>(cpu, inst);
    } else if (ARG1.ArgSize == 16) {
        TaintPropagate_Binop<2>(cpu, inst);
    } else {
        Assert(0);
    }
}

Taint1 TaintEngine::GetTaintAddressingReg( const ARGTYPE &oper ) const
{
    Taint1 t;
    if (oper.Memory.BaseRegister) {
        t |= Shrink(CpuTaint.GPRegs[TranslateReg(oper.Memory.BaseRegister)]);
    }
    if (oper.Memory.IndexRegister) {
        t |= Shrink(CpuTaint.GPRegs[TranslateReg(oper.Memory.IndexRegister)]);
    }
    return t;
}

Taint8 TaintEngine::GetTaint8( const Processor *cpu, const ARGTYPE &oper )
{
    Assert(oper.ArgSize == 64);
    if (IsRegArg(oper) && (REG_TYPE(oper.ArgType) & MMX_REG)) {
        return CpuTaint.MM[TranslateReg(oper)];
    } else if (IsMemoryArg(oper)) {
        return TaintRule_Load<8>(cpu, oper);
    } else {
        LxFatal("GetTaint8() invalid operand\n");
    }
    return Taint8();
}

Taint16 TaintEngine::GetTaint16( const Processor *cpu, const ARGTYPE &oper )
{
    Assert(oper.ArgSize == 128);
    if (IsRegArg(oper) && (REG_TYPE(oper.ArgType) & SSE_REG)) {
        return CpuTaint.XMM[TranslateReg(oper)];
    } else if (IsMemoryArg(oper)) {
        return TaintRule_Load<16>(cpu, oper);
    } else {
        LxFatal("GetTaint16() invalid operand\n");
    }
    return Taint16();
}

void TaintEngine::SetTaint8( const Processor *cpu, const ARGTYPE &oper, const Taint8 &t )
{
    Assert(oper.ArgSize == 64);
    if (IsRegArg(oper) && (REG_TYPE(oper.ArgType) & MMX_REG)) {
        CpuTaint.MM[TranslateReg(oper)] = t;
    } else if (IsMemoryArg(oper)) {
        return TaintRule_Save<8>(cpu, oper, t);
    } else {
        LxFatal("SetTaint8() invalid operand\n");
    }
}

void TaintEngine::SetTaint16( const Processor *cpu, const ARGTYPE &oper, const Taint16 &t )
{
    Assert(oper.ArgSize == 128);
    if (IsRegArg(oper) && (REG_TYPE(oper.ArgType) & SSE_REG)) {
        CpuTaint.XMM[TranslateReg(oper)] = t;
    } else if (IsMemoryArg(oper)) {
        return TaintRule_Save<16>(cpu, oper, t);
    } else {
        LxFatal("SetTaint16() invalid operand\n");
    }
}

void TaintEngine::TaintMemoryRanged( u32 addr, u32 len, bool taintAllBits )
{
    for (uint i = 0; i < len; i++) {
        Taint1 t;
        if (taintAllBits) {
            t.SetAll();
        } else {
            t.T[0].Set(i % t.T[0].GetWidth());
        }
        MemTaint.Set<1>(addr + i, t);
    }
}

TaintEngine::TaintInstHandler TaintEngine::HandlerOneByte[] = {
    /*0x00*/ &TaintEngine::DefaultBinopHandler,
    /*0x01*/ &TaintEngine::DefaultBinopHandler,
    /*0x02*/ &TaintEngine::DefaultBinopHandler,
    /*0x03*/ &TaintEngine::DefaultBinopHandler,
    /*0x04*/ &TaintEngine::DefaultBinopHandler,
    /*0x05*/ &TaintEngine::DefaultBinopHandler,
    /*0x06*/ NULL,
    /*0x07*/ NULL,
    /*0x08*/ &TaintEngine::Or_Handler,
    /*0x09*/ &TaintEngine::Or_Handler,
    /*0x0a*/ &TaintEngine::Or_Handler,
    /*0x0b*/ &TaintEngine::Or_Handler,
    /*0x0c*/ &TaintEngine::Or_Handler,
    /*0x0d*/ &TaintEngine::Or_Handler,
    /*0x0e*/ NULL,
    /*0x0f*/ NULL,
    /*0x10*/ NULL,
    /*0x11*/ NULL,
    /*0x12*/ &TaintEngine::Adc_Sbb_Handler,
    /*0x13*/ &TaintEngine::Adc_Sbb_Handler,
    /*0x14*/ NULL,
    /*0x15*/ NULL,
    /*0x16*/ NULL,
    /*0x17*/ NULL,
    /*0x18*/ NULL,
    /*0x19*/ &TaintEngine::Adc_Sbb_Handler,
    /*0x1a*/ &TaintEngine::Adc_Sbb_Handler,
    /*0x1b*/ &TaintEngine::Adc_Sbb_Handler,
    /*0x1c*/ NULL,
    /*0x1d*/ NULL,
    /*0x1e*/ NULL,
    /*0x1f*/ NULL,
    /*0x20*/ &TaintEngine::And_Handler,
    /*0x21*/ &TaintEngine::And_Handler,
    /*0x22*/ &TaintEngine::And_Handler,
    /*0x23*/ &TaintEngine::And_Handler,
    /*0x24*/ &TaintEngine::And_Handler,
    /*0x25*/ &TaintEngine::And_Handler,
    /*0x26*/ NULL,
    /*0x27*/ NULL,
    /*0x28*/ &TaintEngine::DefaultBinopHandler,
    /*0x29*/ &TaintEngine::DefaultBinopHandler,
    /*0x2a*/ &TaintEngine::DefaultBinopHandler,
    /*0x2b*/ &TaintEngine::DefaultBinopHandler,
    /*0x2c*/ &TaintEngine::DefaultBinopHandler,
    /*0x2d*/ &TaintEngine::DefaultBinopHandler,
    /*0x2e*/ NULL,
    /*0x2f*/ NULL,
    /*0x30*/ &TaintEngine::Xor_Handler,
    /*0x31*/ &TaintEngine::Xor_Handler,
    /*0x32*/ &TaintEngine::Xor_Handler,
    /*0x33*/ &TaintEngine::Xor_Handler,
    /*0x34*/ &TaintEngine::Xor_Handler,
    /*0x35*/ &TaintEngine::Xor_Handler,
    /*0x36*/ NULL,
    /*0x37*/ NULL,
    /*0x38*/ &TaintEngine::Cmp_Test_Handler,
    /*0x39*/ &TaintEngine::Cmp_Test_Handler,
    /*0x3a*/ &TaintEngine::Cmp_Test_Handler,
    /*0x3b*/ &TaintEngine::Cmp_Test_Handler,
    /*0x3c*/ &TaintEngine::Cmp_Test_Handler,
    /*0x3d*/ &TaintEngine::Cmp_Test_Handler,
    /*0x3e*/ NULL,
    /*0x3f*/ NULL,
    /*0x40*/ &TaintEngine::Inc_Dec_Handler,
    /*0x41*/ &TaintEngine::Inc_Dec_Handler,
    /*0x42*/ &TaintEngine::Inc_Dec_Handler,
    /*0x43*/ &TaintEngine::Inc_Dec_Handler,
    /*0x44*/ &TaintEngine::Inc_Dec_Handler,
    /*0x45*/ &TaintEngine::Inc_Dec_Handler,
    /*0x46*/ &TaintEngine::Inc_Dec_Handler,
    /*0x47*/ &TaintEngine::Inc_Dec_Handler,
    /*0x48*/ &TaintEngine::Inc_Dec_Handler,
    /*0x49*/ &TaintEngine::Inc_Dec_Handler,
    /*0x4a*/ &TaintEngine::Inc_Dec_Handler,
    /*0x4b*/ &TaintEngine::Inc_Dec_Handler,
    /*0x4c*/ &TaintEngine::Inc_Dec_Handler,
    /*0x4d*/ &TaintEngine::Inc_Dec_Handler,
    /*0x4e*/ &TaintEngine::Inc_Dec_Handler,
    /*0x4f*/ &TaintEngine::Inc_Dec_Handler,
    /*0x50*/ &TaintEngine::Push_Handler,
    /*0x51*/ &TaintEngine::Push_Handler,
    /*0x52*/ &TaintEngine::Push_Handler,
    /*0x53*/ &TaintEngine::Push_Handler,
    /*0x54*/ &TaintEngine::Push_Handler,
    /*0x55*/ &TaintEngine::Push_Handler,
    /*0x56*/ &TaintEngine::Push_Handler,
    /*0x57*/ &TaintEngine::Push_Handler,
    /*0x58*/ &TaintEngine::Pop_Handler,
    /*0x59*/ &TaintEngine::Pop_Handler,
    /*0x5a*/ &TaintEngine::Pop_Handler,
    /*0x5b*/ &TaintEngine::Pop_Handler,
    /*0x5c*/ &TaintEngine::Pop_Handler,
    /*0x5d*/ &TaintEngine::Pop_Handler,
    /*0x5e*/ &TaintEngine::Pop_Handler,
    /*0x5f*/ &TaintEngine::Pop_Handler,
    /*0x60*/ NULL,
    /*0x61*/ NULL,
    /*0x62*/ NULL,
    /*0x63*/ NULL,
    /*0x64*/ NULL,
    /*0x65*/ NULL,
    /*0x66*/ NULL,
    /*0x67*/ NULL,
    /*0x68*/ &TaintEngine::Push_Handler,
    /*0x69*/ &TaintEngine::Imul69_Imul6B_Handler,
    /*0x6a*/ &TaintEngine::Push_Handler,
    /*0x6b*/ &TaintEngine::Imul69_Imul6B_Handler,
    /*0x6c*/ NULL,
    /*0x6d*/ NULL,
    /*0x6e*/ NULL,
    /*0x6f*/ NULL,
    /*0x70*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::OF>,
    /*0x71*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::OF>,
    /*0x72*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::CF>,
    /*0x73*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::CF>,
    /*0x74*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::ZF>,
    /*0x75*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::ZF>,
    /*0x76*/ &TaintEngine::TaintPropagate_CJmp2<InstContext::CF, InstContext::ZF>,
    /*0x77*/ &TaintEngine::TaintPropagate_CJmp2<InstContext::CF, InstContext::ZF>,
    /*0x78*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::SF>,
    /*0x79*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::SF>,
    /*0x7a*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::PF>,
    /*0x7b*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::PF>,
    /*0x7c*/ &TaintEngine::TaintPropagate_CJmp2<InstContext::SF, InstContext::OF>,
    /*0x7d*/ &TaintEngine::TaintPropagate_CJmp2<InstContext::SF, InstContext::OF>,
    /*0x7e*/ &TaintEngine::TaintPropagate_CJmp3<InstContext::ZF, InstContext::SF, InstContext::OF>,
    /*0x7f*/ &TaintEngine::TaintPropagate_CJmp3<InstContext::ZF, InstContext::SF, InstContext::OF>,
    /*0x80*/ &TaintEngine::Ext80_Handler,
    /*0x81*/ &TaintEngine::Ext81_Handler,
    /*0x82*/ NULL,
    /*0x83*/ &TaintEngine::Ext83_Handler,
    /*0x84*/ &TaintEngine::Cmp_Test_Handler,
    /*0x85*/ &TaintEngine::Cmp_Test_Handler,
    /*0x86*/ NULL,
    /*0x87*/ &TaintEngine::Xchg_Handler,
    /*0x88*/ &TaintEngine::Mov_Handler,
    /*0x89*/ &TaintEngine::Mov_Handler,
    /*0x8a*/ &TaintEngine::Mov_Handler,
    /*0x8b*/ &TaintEngine::Mov_Handler,
    /*0x8c*/ NULL,      // mov r/m16, sreg          not implemented
    /*0x8d*/ &TaintEngine::Lea_Handler,
    /*0x8e*/ NULL,
    /*0x8f*/ &TaintEngine::Ext8F_Handler,
    /*0x90*/ NULL,
    /*0x91*/ &TaintEngine::Xchg_Handler,
    /*0x92*/ &TaintEngine::Xchg_Handler,
    /*0x93*/ &TaintEngine::Xchg_Handler,
    /*0x94*/ &TaintEngine::Xchg_Handler,
    /*0x95*/ &TaintEngine::Xchg_Handler,
    /*0x96*/ &TaintEngine::Xchg_Handler,
    /*0x97*/ &TaintEngine::Xchg_Handler,
    /*0x98*/ &TaintEngine::Cbw_Handler,
    /*0x99*/ &TaintEngine::Cdq_Handler,
    /*0x9a*/ NULL,
    /*0x9b*/ NULL,
    /*0x9c*/ NULL,
    /*0x9d*/ NULL,
    /*0x9e*/ &TaintEngine::Sahf_Handler,
    /*0x9f*/ NULL,
    /*0xa0*/ &TaintEngine::Mov_Handler,
    /*0xa1*/ &TaintEngine::Mov_Handler,
    /*0xa2*/ &TaintEngine::Mov_Handler,
    /*0xa3*/ &TaintEngine::Mov_Handler,
    /*0xa4*/ &TaintEngine::Movs_Handler,
    /*0xa5*/ &TaintEngine::Movs_Handler,
    /*0xa6*/ &TaintEngine::Cmps_Handler,
    /*0xa7*/ &TaintEngine::Cmps_Handler,
    /*0xa8*/ &TaintEngine::Cmp_Test_Handler,
    /*0xa9*/ &TaintEngine::Cmp_Test_Handler,
    /*0xaa*/ &TaintEngine::Stos_Handler,
    /*0xab*/ &TaintEngine::Stos_Handler,
    /*0xac*/ &TaintEngine::Lods_Handler,
    /*0xad*/ &TaintEngine::Lods_Handler,
    /*0xae*/ &TaintEngine::Scas_Handler,
    /*0xaf*/ &TaintEngine::Scas_Handler,
    /*0xb0*/ &TaintEngine::Mov_Handler,
    /*0xb1*/ &TaintEngine::Mov_Handler,
    /*0xb2*/ &TaintEngine::Mov_Handler,
    /*0xb3*/ &TaintEngine::Mov_Handler,
    /*0xb4*/ &TaintEngine::Mov_Handler,
    /*0xb5*/ &TaintEngine::Mov_Handler,
    /*0xb6*/ &TaintEngine::Mov_Handler,
    /*0xb7*/ &TaintEngine::Mov_Handler,
    /*0xb8*/ &TaintEngine::Mov_Handler,
    /*0xb9*/ &TaintEngine::Mov_Handler,
    /*0xba*/ &TaintEngine::Mov_Handler,
    /*0xbb*/ &TaintEngine::Mov_Handler,
    /*0xbc*/ &TaintEngine::Mov_Handler,
    /*0xbd*/ &TaintEngine::Mov_Handler,
    /*0xbe*/ &TaintEngine::Mov_Handler,
    /*0xbf*/ &TaintEngine::Mov_Handler,
    /*0xc0*/ &TaintEngine::ExtC0_Handler,
    /*0xc1*/ &TaintEngine::ExtC1_Handler,
    /*0xc2*/ &TaintEngine::Ret_Handler,
    /*0xc3*/ &TaintEngine::Ret_Handler,
    /*0xc4*/ NULL,
    /*0xc5*/ NULL,
    /*0xc6*/ &TaintEngine::ExtC6_Handler,
    /*0xc7*/ &TaintEngine::ExtC7_Handler,
    /*0xc8*/ NULL,
    /*0xc9*/ NULL,
    /*0xca*/ NULL,
    /*0xcb*/ NULL,
    /*0xcc*/ NULL,
    /*0xcd*/ NULL,
    /*0xce*/ NULL,
    /*0xcf*/ NULL,
    /*0xd0*/ &TaintEngine::ExtD0_Handler,
    /*0xd1*/ &TaintEngine::ExtD1_Handler,
    /*0xd2*/ &TaintEngine::ExtD2_Handler,
    /*0xd3*/ &TaintEngine::ExtD3_Handler,
    /*0xd4*/ NULL,
    /*0xd5*/ NULL,
    /*0xd6*/ NULL,
    /*0xd7*/ NULL,
    /*0xd8*/ NULL,
    /*0xd9*/ NULL,
    /*0xda*/ NULL,
    /*0xdb*/ NULL,
    /*0xdc*/ NULL,
    /*0xdd*/ NULL,
    /*0xde*/ NULL,
    /*0xdf*/ NULL,
    /*0xe0*/ NULL,
    /*0xe1*/ NULL,
    /*0xe2*/ &TaintEngine::LoopE2_JecxzE3_Handler,
    /*0xe3*/ &TaintEngine::LoopE2_JecxzE3_Handler,
    /*0xe4*/ NULL,
    /*0xe5*/ NULL,
    /*0xe6*/ NULL,
    /*0xe7*/ NULL,
    /*0xe8*/ &TaintEngine::Call_Handler,
    /*0xe9*/ &TaintEngine::Jmp_Handler,
    /*0xea*/ NULL,
    /*0xeb*/ &TaintEngine::Jmp_Handler,
    /*0xec*/ NULL,
    /*0xed*/ NULL,
    /*0xee*/ NULL,
    /*0xef*/ NULL,
    /*0xf0*/ NULL,
    /*0xf1*/ NULL,
    /*0xf2*/ NULL,
    /*0xf3*/ NULL,
    /*0xf4*/ NULL,
    /*0xf5*/ NULL,
    /*0xf6*/ &TaintEngine::ExtF6_Handler,
    /*0xf7*/ &TaintEngine::ExtF7_Handler,
    /*0xf8*/ &TaintEngine::TaintPropagate_ClearFlag<InstContext::CF>,
    /*0xf9*/ &TaintEngine::TaintPropagate_ClearFlag<InstContext::CF>,
    /*0xfa*/ NULL,
    /*0xfb*/ NULL,
    /*0xfc*/ NULL,
    /*0xfd*/ NULL,
    /*0xfe*/ &TaintEngine::ExtFE_Handler,
    /*0xff*/ &TaintEngine::ExtFF_Handler,
};

TaintEngine::TaintInstHandler TaintEngine::HandlerTwoBytes[] = {
    /*0x00*/ NULL,
    /*0x01*/ &TaintEngine::ClearEaxEdx_Handler,
    /*0x02*/ NULL,
    /*0x03*/ NULL,
    /*0x04*/ NULL,
    /*0x05*/ NULL,
    /*0x06*/ NULL,
    /*0x07*/ NULL,
    /*0x08*/ NULL,
    /*0x09*/ NULL,
    /*0x0a*/ NULL,
    /*0x0b*/ NULL,
    /*0x0c*/ NULL,
    /*0x0d*/ NULL,
    /*0x0e*/ NULL,
    /*0x0f*/ NULL,
    /*0x10*/ NULL,
    /*0x11*/ NULL,
    /*0x12*/ NULL,
    /*0x13*/ NULL,
    /*0x14*/ NULL,
    /*0x15*/ NULL,
    /*0x16*/ NULL,
    /*0x17*/ NULL,
    /*0x18*/ NULL,
    /*0x19*/ NULL,
    /*0x1a*/ NULL,
    /*0x1b*/ NULL,
    /*0x1c*/ NULL,
    /*0x1d*/ NULL,
    /*0x1e*/ NULL,
    /*0x1f*/ &TaintEngine::Ext0F1F_Handler,
    /*0x20*/ NULL,
    /*0x21*/ NULL,
    /*0x22*/ NULL,
    /*0x23*/ NULL,
    /*0x24*/ NULL,
    /*0x25*/ NULL,
    /*0x26*/ NULL,
    /*0x27*/ NULL,
    /*0x28*/ &TaintEngine::Movapd660F28_Handler,
    /*0x29*/ NULL,
    /*0x2a*/ NULL,
    /*0x2b*/ NULL,
    /*0x2c*/ NULL,
    /*0x2d*/ NULL,
    /*0x2e*/ NULL,
    /*0x2f*/ NULL,
    /*0x30*/ NULL,
    /*0x31*/ &TaintEngine::ClearEaxEdx_Handler,
    /*0x32*/ NULL,
    /*0x33*/ NULL,
    /*0x34*/ NULL,
    /*0x35*/ NULL,
    /*0x36*/ NULL,
    /*0x37*/ NULL,
    /*0x38*/ NULL,
    /*0x39*/ NULL,
    /*0x3a*/ NULL,
    /*0x3b*/ NULL,
    /*0x3c*/ NULL,
    /*0x3d*/ NULL,
    /*0x3e*/ NULL,
    /*0x3f*/ NULL,
    /*0x40*/ NULL,
    /*0x41*/ NULL,
    /*0x42*/ NULL,
    /*0x43*/ NULL,
    /*0x44*/ NULL,
    /*0x45*/ NULL,
    /*0x46*/ NULL,
    /*0x47*/ NULL,
    /*0x48*/ NULL,
    /*0x49*/ NULL,
    /*0x4a*/ NULL,
    /*0x4b*/ NULL,
    /*0x4c*/ NULL,
    /*0x4d*/ NULL,
    /*0x4e*/ NULL,
    /*0x4f*/ NULL,
    /*0x50*/ NULL,
    /*0x51*/ NULL,
    /*0x52*/ NULL,
    /*0x53*/ NULL,
    /*0x54*/ NULL,
    /*0x55*/ NULL,
    /*0x56*/ NULL,
    /*0x57*/ NULL,
    /*0x58*/ NULL,
    /*0x59*/ NULL,
    /*0x5a*/ NULL,
    /*0x5b*/ NULL,
    /*0x5c*/ NULL,
    /*0x5d*/ NULL,
    /*0x5e*/ NULL,
    /*0x5f*/ NULL,
    /*0x60*/ NULL,
    /*0x61*/ NULL,
    /*0x62*/ NULL,
    /*0x63*/ NULL,
    /*0x64*/ NULL,
    /*0x65*/ NULL,
    /*0x66*/ NULL,
    /*0x67*/ NULL,
    /*0x68*/ NULL,
    /*0x69*/ NULL,
    /*0x6a*/ NULL,
    /*0x6b*/ NULL,
    /*0x6c*/ NULL,
    /*0x6d*/ NULL,
    /*0x6e*/ NULL,
    /*0x6f*/ &TaintEngine::Movdqa0F6F_7F_Handler,
    /*0x70*/ NULL,
    /*0x71*/ NULL,
    /*0x72*/ NULL,
    /*0x73*/ NULL,
    /*0x74*/ NULL,
    /*0x75*/ NULL,
    /*0x76*/ NULL,
    /*0x77*/ NULL,
    /*0x78*/ NULL,
    /*0x79*/ NULL,
    /*0x7a*/ NULL,
    /*0x7b*/ NULL,
    /*0x7c*/ NULL,
    /*0x7d*/ NULL,
    /*0x7e*/ &TaintEngine::Movd0F7E_Handler,
    /*0x7f*/ &TaintEngine::Movdqa0F6F_7F_Handler,
    /*0x80*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::OF>,
    /*0x81*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::OF>,
    /*0x82*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::CF>,
    /*0x83*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::CF>,
    /*0x84*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::ZF>,
    /*0x85*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::ZF>,
    /*0x86*/ &TaintEngine::TaintPropagate_CJmp2<InstContext::CF, InstContext::ZF>,
    /*0x87*/ &TaintEngine::TaintPropagate_CJmp2<InstContext::CF, InstContext::ZF>,
    /*0x88*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::SF>,
    /*0x89*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::SF>,
    /*0x8a*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::PF>,
    /*0x8b*/ &TaintEngine::TaintPropagate_CJmp1<InstContext::PF>,
    /*0x8c*/ &TaintEngine::TaintPropagate_CJmp2<InstContext::SF, InstContext::OF>,
    /*0x8d*/ &TaintEngine::TaintPropagate_CJmp2<InstContext::SF, InstContext::OF>,
    /*0x8e*/ &TaintEngine::TaintPropagate_CJmp3<InstContext::ZF, InstContext::SF, InstContext::OF>,
    /*0x8f*/ &TaintEngine::TaintPropagate_CJmp3<InstContext::ZF, InstContext::SF, InstContext::OF>,

    /*0x90*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::OF>,
    /*0x91*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::OF>,
    /*0x92*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::CF>,
    /*0x93*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::CF>,
    /*0x94*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::ZF>,
    /*0x95*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::ZF>,
    /*0x96*/ &TaintEngine::TaintPropagate_Setcc2<InstContext::CF, InstContext::ZF>,
    /*0x97*/ &TaintEngine::TaintPropagate_Setcc2<InstContext::CF, InstContext::ZF>,
    /*0x98*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::SF>,
    /*0x99*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::SF>,
    /*0x9a*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::PF>,
    /*0x9b*/ &TaintEngine::TaintPropagate_Setcc1<InstContext::PF>,
    /*0x9c*/ &TaintEngine::TaintPropagate_Setcc2<InstContext::SF, InstContext::OF>,
    /*0x9d*/ &TaintEngine::TaintPropagate_Setcc2<InstContext::SF, InstContext::OF>,
    /*0x9e*/ &TaintEngine::TaintPropagate_Setcc3<InstContext::ZF, InstContext::SF, InstContext::OF>,
    /*0x9f*/ &TaintEngine::TaintPropagate_Setcc3<InstContext::ZF, InstContext::SF, InstContext::OF>,
    /*0xa0*/ NULL,
    /*0xa1*/ NULL,
    /*0xa2*/ &TaintEngine::Cpuid_Handler,
    /*0xa3*/ &TaintEngine::Bt_Handler,
    /*0xa4*/ &TaintEngine::Shld_Shrd_Handler,
    /*0xa5*/ &TaintEngine::Shld_Shrd_Handler,
    /*0xa6*/ NULL,
    /*0xa7*/ NULL,
    /*0xa8*/ NULL,
    /*0xa9*/ NULL,
    /*0xaa*/ NULL,
    /*0xab*/ NULL,
    /*0xac*/ &TaintEngine::Shld_Shrd_Handler,
    /*0xad*/ &TaintEngine::Shld_Shrd_Handler,
    /*0xae*/ &TaintEngine::Ext0FAE_Handler,
    /*0xaf*/ &TaintEngine::DefaultBinopHandler, // imul r32, r/m32
    /*0xb0*/ NULL,
    /*0xb1*/ &TaintEngine::Cmpxchg0FB1_Handler,
    /*0xb2*/ NULL,
    /*0xb3*/ NULL,
    /*0xb4*/ NULL,
    /*0xb5*/ NULL,
    /*0xb6*/ &TaintEngine::Movzx_Handler,
    /*0xb7*/ &TaintEngine::Movzx_Handler,
    /*0xb8*/ NULL,
    /*0xb9*/ NULL,
    /*0xba*/ &TaintEngine::Ext0FBA_Handler,
    /*0xbb*/ NULL,
    /*0xbc*/ NULL,
    /*0xbd*/ NULL,
    /*0xbe*/ &TaintEngine::Movsx_Handler,
    /*0xbf*/ &TaintEngine::Movsx_Handler,
    /*0xc0*/ NULL,
    /*0xc1*/ NULL,
    /*0xc2*/ NULL,
    /*0xc3*/ NULL,
    /*0xc4*/ NULL,
    /*0xc5*/ NULL,
    /*0xc6*/ NULL,
    /*0xc7*/ NULL,
    /*0xc8*/ &TaintEngine::Bswap_Handler,
    /*0xc9*/ &TaintEngine::Bswap_Handler,
    /*0xca*/ &TaintEngine::Bswap_Handler,
    /*0xcb*/ &TaintEngine::Bswap_Handler,
    /*0xcc*/ &TaintEngine::Bswap_Handler,
    /*0xcd*/ &TaintEngine::Bswap_Handler,
    /*0xce*/ &TaintEngine::Bswap_Handler,
    /*0xcf*/ &TaintEngine::Bswap_Handler,
    /*0xd0*/ NULL,
    /*0xd1*/ NULL,
    /*0xd2*/ NULL,
    /*0xd3*/ NULL,
    /*0xd4*/ NULL,
    /*0xd5*/ NULL,
    /*0xd6*/ NULL,
    /*0xd7*/ NULL,
    /*0xd8*/ NULL,
    /*0xd9*/ NULL,
    /*0xda*/ NULL,
    /*0xdb*/ NULL,
    /*0xdc*/ NULL,
    /*0xdd*/ NULL,
    /*0xde*/ NULL,
    /*0xdf*/ NULL,
    /*0xe0*/ NULL,
    /*0xe1*/ NULL,
    /*0xe2*/ NULL,
    /*0xe3*/ NULL,
    /*0xe4*/ NULL,
    /*0xe5*/ NULL,
    /*0xe6*/ NULL,
    /*0xe7*/ NULL,
    /*0xe8*/ NULL,
    /*0xe9*/ NULL,
    /*0xea*/ NULL,
    /*0xeb*/ NULL,
    /*0xec*/ NULL,
    /*0xed*/ NULL,
    /*0xee*/ NULL,
    /*0xef*/ &TaintEngine::Pxor660FEF_Handler,
    /*0xf0*/ NULL,
    /*0xf1*/ NULL,
    /*0xf2*/ NULL,
    /*0xf3*/ NULL,
    /*0xf4*/ NULL,
    /*0xf5*/ NULL,
    /*0xf6*/ NULL,
    /*0xf7*/ NULL,
    /*0xf8*/ NULL,
    /*0xf9*/ NULL,
    /*0xfa*/ NULL,
    /*0xfb*/ NULL,
    /*0xfc*/ NULL,
    /*0xfd*/ NULL,
    /*0xfe*/ NULL,
    /*0xff*/ NULL,
};

// void TaintEngine::Ext80_Handler(const Processor *cpu, const Instruction *inst)
// {
//     static TaintInstHandler handlers[] = {
//         /* 0 */ NULL,
//         /* 1 */ NULL,
//         /* 2 */ NULL,
//         /* 3 */ NULL,
//         /* 4 */ NULL,
//         /* 5 */ NULL,
//         /* 6 */ NULL,
//         /* 7 */ NULL,
//     };
//     return (this->*(handlers[MASK_MODRM_REG(inst->Aux.modrm)]))(cpu, inst);
// }
void TaintEngine::Ext80_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::DefaultBinopHandler,
        /* 1 */ &TaintEngine::Or_Handler,
        /* 2 */ NULL,
        /* 3 */ NULL,
        /* 4 */ &TaintEngine::And_Handler,
        /* 5 */ &TaintEngine::DefaultBinopHandler,
        /* 6 */ &TaintEngine::Xor_Handler,
        /* 7 */ &TaintEngine::Cmp_Test_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::Ext81_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::DefaultBinopHandler,
        /* 1 */ &TaintEngine::Or_Handler,
        /* 2 */ &TaintEngine::Adc_Sbb_Handler,
        /* 3 */ NULL,
        /* 4 */ &TaintEngine::And_Handler,
        /* 5 */ &TaintEngine::DefaultBinopHandler,
        /* 6 */ &TaintEngine::Xor_Handler,
        /* 7 */ &TaintEngine::Cmp_Test_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::Ext83_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::DefaultBinopHandler,
        /* 1 */ &TaintEngine::Or_Handler,
        /* 2 */ &TaintEngine::Adc_Sbb_Handler,
        /* 3 */ &TaintEngine::Adc_Sbb_Handler,
        /* 4 */ &TaintEngine::And_Handler,
        /* 5 */ &TaintEngine::DefaultBinopHandler,
        /* 6 */ &TaintEngine::Xor_Handler,
        /* 7 */ &TaintEngine::Cmp_Test_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::Ext8F_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::Pop_Handler,
        /* 1 */ NULL,
        /* 2 */ NULL,
        /* 3 */ NULL,
        /* 4 */ NULL,
        /* 5 */ NULL,
        /* 6 */ NULL,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtC0_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::ShiftRotate_Handler,
        /* 1 */ &TaintEngine::ShiftRotate_Handler,
        /* 2 */ &TaintEngine::ShiftRotate_Handler,
        /* 3 */ &TaintEngine::ShiftRotate_Handler,
        /* 4 */ &TaintEngine::ShiftRotate_Handler,
        /* 5 */ &TaintEngine::ShiftRotate_Handler,
        /* 6 */ NULL,
        /* 7 */ &TaintEngine::ShiftRotate_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtC1_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::ShiftRotate_Handler,
        /* 1 */ &TaintEngine::ShiftRotate_Handler,
        /* 2 */ &TaintEngine::ShiftRotate_Handler,
        /* 3 */ &TaintEngine::ShiftRotate_Handler,
        /* 4 */ &TaintEngine::ShiftRotate_Handler,
        /* 5 */ &TaintEngine::ShiftRotate_Handler,
        /* 6 */ NULL,
        /* 7 */ &TaintEngine::ShiftRotate_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtC6_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::Mov_Handler,
        /* 1 */ NULL,
        /* 2 */ NULL,
        /* 3 */ NULL,
        /* 4 */ NULL,
        /* 5 */ NULL,
        /* 6 */ NULL,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtC7_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::Mov_Handler,
        /* 1 */ NULL,
        /* 2 */ NULL,
        /* 3 */ NULL,
        /* 4 */ NULL,
        /* 5 */ NULL,
        /* 6 */ NULL,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtD0_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::ShiftRotate_Handler,
        /* 1 */ &TaintEngine::ShiftRotate_Handler,
        /* 2 */ &TaintEngine::ShiftRotate_Handler,
        /* 3 */ &TaintEngine::ShiftRotate_Handler,
        /* 4 */ &TaintEngine::ShiftRotate_Handler,
        /* 5 */ &TaintEngine::ShiftRotate_Handler,
        /* 6 */ NULL,
        /* 7 */ &TaintEngine::ShiftRotate_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtD1_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::ShiftRotate_Handler,
        /* 1 */ &TaintEngine::ShiftRotate_Handler,
        /* 2 */ &TaintEngine::ShiftRotate_Handler,
        /* 3 */ &TaintEngine::ShiftRotate_Handler,
        /* 4 */ &TaintEngine::ShiftRotate_Handler,
        /* 5 */ &TaintEngine::ShiftRotate_Handler,
        /* 6 */ NULL,
        /* 7 */ &TaintEngine::ShiftRotate_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtD2_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::ShiftRotate_Handler,
        /* 1 */ &TaintEngine::ShiftRotate_Handler,
        /* 2 */ &TaintEngine::ShiftRotate_Handler,
        /* 3 */ &TaintEngine::ShiftRotate_Handler,
        /* 4 */ &TaintEngine::ShiftRotate_Handler,
        /* 5 */ &TaintEngine::ShiftRotate_Handler,
        /* 6 */ NULL,
        /* 7 */ &TaintEngine::ShiftRotate_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtD3_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::ShiftRotate_Handler,
        /* 1 */ &TaintEngine::ShiftRotate_Handler,
        /* 2 */ &TaintEngine::ShiftRotate_Handler,
        /* 3 */ &TaintEngine::ShiftRotate_Handler,
        /* 4 */ &TaintEngine::ShiftRotate_Handler,
        /* 5 */ &TaintEngine::ShiftRotate_Handler,
        /* 6 */ NULL,
        /* 7 */ &TaintEngine::ShiftRotate_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtF6_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::Cmp_Test_Handler,
        /* 1 */ NULL,
        /* 2 */ NULL,
        /* 3 */ &TaintEngine::Neg_Handler,
        /* 4 */ &TaintEngine::ImulF6_MulF6_Handler,
        /* 5 */ &TaintEngine::ImulF6_MulF6_Handler,
        /* 6 */ NULL,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtF7_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::Cmp_Test_Handler,
        /* 1 */ NULL,
        /* 2 */ NULL,
        /* 3 */ &TaintEngine::Neg_Handler,
        /* 4 */ &TaintEngine::ImulF6_MulF6_Handler,
        /* 5 */ &TaintEngine::ImulF7_MulF6_Handler,
        /* 6 */ &TaintEngine::DivF7_IdivF7_Handler,
        /* 7 */ &TaintEngine::DivF7_IdivF7_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtFE_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::Inc_Dec_Handler,
        /* 1 */ &TaintEngine::Inc_Dec_Handler,
        /* 2 */ NULL,
        /* 3 */ NULL,
        /* 4 */ NULL,
        /* 5 */ NULL,
        /* 6 */ NULL,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::ExtFF_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ &TaintEngine::Inc_Dec_Handler,
        /* 1 */ &TaintEngine::Inc_Dec_Handler,
        /* 2 */ &TaintEngine::Call_Handler,
        /* 3 */ NULL,
        /* 4 */ &TaintEngine::Jmp_Handler,
        /* 5 */ NULL,
        /* 6 */ &TaintEngine::Push_Handler,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::Ext0F1F_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ NULL,
        /* 1 */ NULL,
        /* 2 */ NULL,
        /* 3 */ NULL,
        /* 4 */ NULL,
        /* 5 */ NULL,
        /* 6 */ NULL,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::Ext0FAE_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ NULL,
        /* 1 */ NULL,
        /* 2 */ NULL,
        /* 3 */ &TaintEngine::TaintPropagate_ClearDest<4>,
        /* 4 */ NULL,
        /* 5 */ NULL,
        /* 6 */ NULL,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::Ext0FBA_Handler(const Processor *cpu, const Instruction *inst)
{
    static TaintInstHandler handlers[] = {
        /* 0 */ NULL,
        /* 1 */ NULL,
        /* 2 */ NULL,
        /* 3 */ NULL,
        /* 4 */ &TaintEngine::Bt_Handler,
        /* 5 */ NULL,
        /* 6 */ NULL,
        /* 7 */ NULL,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}


#define DEFINE_CUSTOM_BINOP_HANDLER(HandlerName, PropagateTmpl)                    \
void TaintEngine::HandlerName(const Processor *cpu, const Instruction *inst)    \
{                                                                               \
    /*Assert(ARG1.ArgSize == ARG2.ArgSize); */                                  \
    if (ARG1.ArgSize == 32) {                                                   \
        PropagateTmpl<4>(cpu, inst);                                            \
    } else if (ARG1.ArgSize == 8) {                                             \
        PropagateTmpl<1>(cpu, inst);                                            \
    } else if (ARG1.ArgSize == 16) {                                            \
        PropagateTmpl<2>(cpu, inst);                                            \
    } else {                                                                    \
        Assert(0);                                                              \
    }                                                                           \
}                                                                               \


DEFINE_CUSTOM_BINOP_HANDLER(Or_Handler,             TaintPropagate_Or)
DEFINE_CUSTOM_BINOP_HANDLER(Adc_Sbb_Handler,        TaintPropagate_Adc_Sbb)
DEFINE_CUSTOM_BINOP_HANDLER(And_Handler,            TaintPropagate_And)
DEFINE_CUSTOM_BINOP_HANDLER(Xor_Handler,            TaintPropagate_Xor)
DEFINE_CUSTOM_BINOP_HANDLER(Cmp_Test_Handler,       TaintPropagate_Cmp_Test)
DEFINE_CUSTOM_BINOP_HANDLER(Inc_Dec_Handler,        TaintPropagate_Inc_Dec)
DEFINE_CUSTOM_BINOP_HANDLER(Push_Handler,           TaintPropagate_Push)
DEFINE_CUSTOM_BINOP_HANDLER(Pop_Handler,            TaintPropagate_Pop)
DEFINE_CUSTOM_BINOP_HANDLER(Imul69_Imul6B_Handler,  TaintPropagate_Imul)
DEFINE_CUSTOM_BINOP_HANDLER(Xchg_Handler,           TaintPropagate_Xchg)
DEFINE_CUSTOM_BINOP_HANDLER(Mov_Handler,            TaintPropagate_Mov)
DEFINE_CUSTOM_BINOP_HANDLER(Lea_Handler,            TaintPropagate_Lea)
DEFINE_CUSTOM_BINOP_HANDLER(Movs_Handler,           TaintPropagate_Movs)
DEFINE_CUSTOM_BINOP_HANDLER(Cmps_Handler,           TaintPropagate_Cmps)
DEFINE_CUSTOM_BINOP_HANDLER(Stos_Handler,           TaintPropagate_Stos)
DEFINE_CUSTOM_BINOP_HANDLER(Lods_Handler,           TaintPropagate_Lods)
DEFINE_CUSTOM_BINOP_HANDLER(Scas_Handler,           TaintPropagate_Scas)
DEFINE_CUSTOM_BINOP_HANDLER(ShiftRotate_Handler,    TaintPropagate_ShiftRotate)
DEFINE_CUSTOM_BINOP_HANDLER(Neg_Handler,            TaintPropagate_Neg)
DEFINE_CUSTOM_BINOP_HANDLER(Bt_Handler,             TaintPropagate_Bt)
DEFINE_CUSTOM_BINOP_HANDLER(Xadd_Handler,           TaintPropagate_Xadd)

void TaintEngine::ImulF6_MulF6_Handler(const Processor *cpu, const Instruction *inst)
{
    Taint1 t = TaintRule_Binop(FromTaint<4, 1>(CpuTaint.GPRegs[LX_REG_EAX]), GetTaint<1>(cpu, ARG2));
    ToTaint<2, 4>(CpuTaint.GPRegs[LX_REG_EAX], Extend<2>(t), 0);
    SetFlagTaint<1>(inst, t);
}

void TaintEngine::ImulF7_MulF6_Handler(const Processor *cpu, const Instruction *inst)
{
    Taint4 t = TaintRule_Binop<4>(CpuTaint.GPRegs[LX_REG_EAX], GetTaint<4>(cpu, ARG2));
    CpuTaint.GPRegs[LX_REG_EAX] = t;
    CpuTaint.GPRegs[LX_REG_EDX] = t;
    SetFlagTaint<4>(inst, t);
}

void TaintEngine::LoopE2_JecxzE3_Handler(const Processor *cpu, const Instruction *inst)
{
    TaintRule_ConditionalEip(Shrink(CpuTaint.GPRegs[LX_REG_ECX]));
}

void TaintEngine::Cbw_Handler(const Processor *cpu, const Instruction *inst)
{
    Taint4 &eax = CpuTaint.GPRegs[LX_REG_EAX];
    if (inst->Main.Prefix.OperandSize) {
        eax.T[1] = eax.T[0];
    } else {
        Taint t = eax.T[0] | eax.T[1];
        eax.T[2] = t;
        eax.T[3] = t;
    }
}

void TaintEngine::Cdq_Handler(const Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize)
        NOT_IMPLEMENTED();
    CpuTaint.GPRegs[LX_REG_EDX] = CpuTaint.GPRegs[LX_REG_EAX];
}

void TaintEngine::Sahf_Handler(const Processor *cpu, const Instruction *inst)
{
    Taint1 t = FromTaint<4, 1>(CpuTaint.GPRegs[LX_REG_EAX], 1);   // AH
    CpuTaint.Flags[InstContext::CF] = t;
    CpuTaint.Flags[InstContext::PF] = t;
    CpuTaint.Flags[InstContext::AF] = t;
    CpuTaint.Flags[InstContext::ZF] = t;
    CpuTaint.Flags[InstContext::SF] = t;
}

void TaintEngine::Ret_Handler(const Processor *cpu, const Instruction *inst)
{
    u32 imm         = (u32) inst->Main.Inst.Immediat;
    Taint4 t        = MemTaint.Get<4>(cpu->ESP - 4 - imm);
    CpuTaint.Eip    = Shrink(t);
}

void TaintEngine::Call_Handler(const Processor *cpu, const Instruction *inst)
{
    Taint4 t        = Extend<4>(CpuTaint.Eip);
    MemTaint.Set<4>(cpu->ESP, t);
    Taint4 newT     = GetTaint<4>(cpu, ARG1);
    CpuTaint.Eip    = Shrink(newT);
}

void TaintEngine::Jmp_Handler(const Processor *cpu, const Instruction *inst)
{
    Taint4 t        = GetTaint<4>(cpu, ARG1);
    CpuTaint.Eip    = Shrink(t);
}

void TaintEngine::DivF7_IdivF7_Handler(const Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) 
        NOT_IMPLEMENTED();

    Taint4 t1       = CpuTaint.GPRegs[LX_REG_EAX] | CpuTaint.GPRegs[LX_REG_EDX];
    Taint4 t2       = GetTaint<4>(cpu, ARG2);
    Taint4 t        = TaintRule_Binop(t1, t2);
    CpuTaint.GPRegs[LX_REG_EAX] = t;
    CpuTaint.GPRegs[LX_REG_EDX] = t;
}


void TaintEngine::Movapd660F28_Handler(const Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        Taint16 t   = GetTaint16(cpu, ARG2);
        SetTaint16(cpu, ARG1, t);
    } else {
        NOT_IMPLEMENTED();
    }
}

void TaintEngine::Movdqa0F6F_7F_Handler(const Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        Taint16 t   = GetTaint16(cpu, ARG2);
        SetTaint16(cpu, ARG1, t);
    } else {
        Taint8 t    = GetTaint8(cpu, ARG2);
        SetTaint8(cpu, ARG1, t);
    }
}

void TaintEngine::Movd0F7E_Handler(const Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        Taint16 t   = GetTaint16(cpu, ARG2);
        Taint4 s    = FromTaint<16, 4>(t, 0);
        SetTaint<4>(cpu, ARG1, s);
    } else {
        Taint8 t    = GetTaint8(cpu, ARG2);
        Taint4 s    = FromTaint<8, 4>(t, 0);
        SetTaint<4>(cpu, ARG1, s);
    }
}

void TaintEngine::ClearEaxEdx_Handler(const Processor *cpu, const Instruction *inst)
{
    CpuTaint.GPRegs[LX_REG_EAX].ResetAll();
    CpuTaint.GPRegs[LX_REG_EDX].ResetAll();
}

void TaintEngine::Cpuid_Handler(const Processor *cpu, const Instruction *inst)
{
    CpuTaint.GPRegs[LX_REG_EAX].ResetAll();
    CpuTaint.GPRegs[LX_REG_ECX].ResetAll();
    CpuTaint.GPRegs[LX_REG_EDX].ResetAll();
    CpuTaint.GPRegs[LX_REG_EBX].ResetAll();
}

void TaintEngine::Shld_Shrd_Handler(const Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    Taint4 t = TaintRule_Binop(GetTaint<4>(cpu, ARG1), GetTaint<4>(cpu, ARG2));
    if (!IsConstantArg(ARG3)) {
        t |= Extend<4>(FromTaint<4, 1>(CpuTaint.GPRegs[LX_REG_ECX], 0));
    }

    SetTaint<4>(cpu, ARG1, t);
    SetFlagTaint<4>(inst, t);
}

void TaintEngine::Cmpxchg0FB1_Handler(const Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        Taint2 t1 = GetTaint<2>(cpu, ARG1);
        Taint2 t2 = GetTaint<2>(cpu, ARG2);
        Taint2 ax = FromTaint<4, 2>(CpuTaint.GPRegs[LX_REG_EAX], 0);
        if (cpu->ZF == 1) {
            SetTaint<2>(cpu, ARG1, t2);
        } else {
            ToTaint<2, 4>(CpuTaint.GPRegs[LX_REG_EAX], t1);
        }
        CpuTaint.Flags[InstContext::ZF] = Shrink(TaintRule_Binop(ax, t1));
    } else {
        Taint4 t1 = GetTaint<4>(cpu, ARG1);
        Taint4 t2 = GetTaint<4>(cpu, ARG2);
        Taint4 eax = CpuTaint.GPRegs[LX_REG_EAX];
        if (cpu->ZF == 1) {
            SetTaint<4>(cpu, ARG1, t2);
        } else {
            CpuTaint.GPRegs[LX_REG_EAX] = t1;
        }
        CpuTaint.Flags[InstContext::ZF] = Shrink(TaintRule_Binop(eax, t1));
    }
}

void TaintEngine::Movzx_Handler(const Processor *cpu, const Instruction *inst)
{
    if (ARG2.ArgSize == 8) {
        if (ARG1.ArgSize == 32) {
            Taint4 t;
            ToTaint<1, 4>(t, GetTaint<1>(cpu, ARG2));
            SetTaint<4>(cpu, ARG1, t);
        } else {
            Taint2 t;
            ToTaint<1, 2>(t, GetTaint<1>(cpu, ARG2));
            SetTaint<2>(cpu, ARG1, t);
        }
    } else {
        Taint4 t;
        ToTaint<2, 4>(t, GetTaint<2>(cpu, ARG2));
        SetTaint<4>(cpu, ARG1, t);
    }
}

void TaintEngine::Movsx_Handler(const Processor *cpu, const Instruction *inst)
{
    if (ARG2.ArgSize == 8) {
        if (ARG1.ArgSize == 32) {
            Taint4 t = Extend<4>(GetTaint<1>(cpu, ARG2));
            SetTaint<4>(cpu, ARG1, t);
        } else {
            Taint2 t = Extend<2>(GetTaint<1>(cpu, ARG2));
            SetTaint<2>(cpu, ARG1, t);
        }
    } else {
        Taint4 t = Extend<4>(Shrink<2>(GetTaint<2>(cpu, ARG2)));
        SetTaint<4>(cpu, ARG1, t);
    }
}

void TaintEngine::Bswap_Handler(const Processor *cpu, const Instruction *inst)
{
    Taint4 t = GetTaint<4>(cpu, ARG1);
    Taint4 r;
    r.T[0] = t.T[3];
    r.T[1] = t.T[2];
    r.T[2] = t.T[1];
    r.T[3] = t.T[0];
    SetTaint<4>(cpu, ARG1, t);
}

void TaintEngine::Pxor660FEF_Handler(const Processor *cpu, const Instruction *inst)
{
    if (inst->Main.Prefix.OperandSize) {
        Taint16 t = TaintRule_Binop<16>(GetTaint16(cpu, ARG1), GetTaint16(cpu, ARG2));
        SetTaint16(cpu, ARG1, t);
    } else {
        NOT_IMPLEMENTED();
    }
}

