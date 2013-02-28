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

    TaintInstHandler h;
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
    /*0x38*/ &TaintEngine::Cmp_Handler,
    /*0x39*/ &TaintEngine::Cmp_Handler,
    /*0x3a*/ &TaintEngine::Cmp_Handler,
    /*0x3b*/ &TaintEngine::Cmp_Handler,
    /*0x3c*/ &TaintEngine::Cmp_Handler,
    /*0x3d*/ &TaintEngine::Cmp_Handler,
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
    /*0x6f*/ NULL,
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
    /*0x7e*/ NULL,
    /*0x7f*/ NULL,
    /*0x80*/ &TaintEngine::Ext80_Handler,
    /*0x81*/ &TaintEngine::Ext81_Handler,
    /*0x82*/ NULL,
    /*0x83*/ &TaintEngine::Ext83_Handler,
    /*0x84*/ NULL,
    /*0x85*/ NULL,
    /*0x86*/ NULL,
    /*0x87*/ NULL,
    /*0x88*/ NULL,
    /*0x89*/ NULL,
    /*0x8a*/ NULL,
    /*0x8b*/ NULL,
    /*0x8c*/ NULL,
    /*0x8d*/ NULL,
    /*0x8e*/ NULL,
    /*0x8f*/ &TaintEngine::Ext8F_Handler,
    /*0x90*/ NULL,
    /*0x91*/ NULL,
    /*0x92*/ NULL,
    /*0x93*/ NULL,
    /*0x94*/ NULL,
    /*0x95*/ NULL,
    /*0x96*/ NULL,
    /*0x97*/ NULL,
    /*0x98*/ NULL,
    /*0x99*/ NULL,
    /*0x9a*/ NULL,
    /*0x9b*/ NULL,
    /*0x9c*/ NULL,
    /*0x9d*/ NULL,
    /*0x9e*/ NULL,
    /*0x9f*/ NULL,
    /*0xa0*/ NULL,
    /*0xa1*/ NULL,
    /*0xa2*/ NULL,
    /*0xa3*/ NULL,
    /*0xa4*/ NULL,
    /*0xa5*/ NULL,
    /*0xa6*/ NULL,
    /*0xa7*/ NULL,
    /*0xa8*/ NULL,
    /*0xa9*/ NULL,
    /*0xaa*/ NULL,
    /*0xab*/ NULL,
    /*0xac*/ NULL,
    /*0xad*/ NULL,
    /*0xae*/ NULL,
    /*0xaf*/ NULL,
    /*0xb0*/ NULL,
    /*0xb1*/ NULL,
    /*0xb2*/ NULL,
    /*0xb3*/ NULL,
    /*0xb4*/ NULL,
    /*0xb5*/ NULL,
    /*0xb6*/ NULL,
    /*0xb7*/ NULL,
    /*0xb8*/ &TaintEngine::DebugTaintIntroduce,
    /*0xb9*/ &TaintEngine::DebugTaintIntroduce,
    /*0xba*/ &TaintEngine::DebugTaintIntroduce,
    /*0xbb*/ &TaintEngine::DebugTaintIntroduce,
    /*0xbc*/ &TaintEngine::DebugTaintIntroduce,
    /*0xbd*/ &TaintEngine::DebugTaintIntroduce,
    /*0xbe*/ &TaintEngine::DebugTaintIntroduce,
    /*0xbf*/ &TaintEngine::DebugTaintIntroduce,
    /*0xc0*/ &TaintEngine::ExtC0_Handler,
    /*0xc1*/ &TaintEngine::ExtC1_Handler,
    /*0xc2*/ NULL,
    /*0xc3*/ NULL,
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
    /*0xef*/ NULL,
    /*0xf0*/ NULL,
    /*0xf1*/ NULL,
    /*0xf2*/ NULL,
    /*0xf3*/ NULL,
    /*0xf4*/ NULL,
    /*0xf5*/ NULL,
    /*0xf6*/ &TaintEngine::ExtF6_Handler,
    /*0xf7*/ &TaintEngine::ExtF7_Handler,
    /*0xf8*/ NULL,
    /*0xf9*/ NULL,
    /*0xfa*/ NULL,
    /*0xfb*/ NULL,
    /*0xfc*/ NULL,
    /*0xfd*/ NULL,
    /*0xfe*/ &TaintEngine::ExtFE_Handler,
    /*0xff*/ &TaintEngine::ExtFF_Handler,
};

TaintEngine::TaintInstHandler TaintEngine::HandlerTwoBytes[] = {
    /*0x00*/ NULL,
    /*0x01*/ NULL,
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
    /*0x28*/ NULL,
    /*0x29*/ NULL,
    /*0x2a*/ NULL,
    /*0x2b*/ NULL,
    /*0x2c*/ NULL,
    /*0x2d*/ NULL,
    /*0x2e*/ NULL,
    /*0x2f*/ NULL,
    /*0x30*/ NULL,
    /*0x31*/ NULL,
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
    /*0x6f*/ NULL,
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
    /*0x7e*/ NULL,
    /*0x7f*/ NULL,
    /*0x80*/ NULL,
    /*0x81*/ NULL,
    /*0x82*/ NULL,
    /*0x83*/ NULL,
    /*0x84*/ NULL,
    /*0x85*/ NULL,
    /*0x86*/ NULL,
    /*0x87*/ NULL,
    /*0x88*/ NULL,
    /*0x89*/ NULL,
    /*0x8a*/ NULL,
    /*0x8b*/ NULL,
    /*0x8c*/ NULL,
    /*0x8d*/ NULL,
    /*0x8e*/ NULL,
    /*0x8f*/ NULL,
    /*0x90*/ NULL,
    /*0x91*/ NULL,
    /*0x92*/ NULL,
    /*0x93*/ NULL,
    /*0x94*/ NULL,
    /*0x95*/ NULL,
    /*0x96*/ NULL,
    /*0x97*/ NULL,
    /*0x98*/ NULL,
    /*0x99*/ NULL,
    /*0x9a*/ NULL,
    /*0x9b*/ NULL,
    /*0x9c*/ NULL,
    /*0x9d*/ NULL,
    /*0x9e*/ NULL,
    /*0x9f*/ NULL,
    /*0xa0*/ NULL,
    /*0xa1*/ NULL,
    /*0xa2*/ NULL,
    /*0xa3*/ NULL,
    /*0xa4*/ NULL,
    /*0xa5*/ NULL,
    /*0xa6*/ NULL,
    /*0xa7*/ NULL,
    /*0xa8*/ NULL,
    /*0xa9*/ NULL,
    /*0xaa*/ NULL,
    /*0xab*/ NULL,
    /*0xac*/ NULL,
    /*0xad*/ NULL,
    /*0xae*/ &TaintEngine::Ext0FAE_Handler,
    /*0xaf*/ NULL,
    /*0xb0*/ NULL,
    /*0xb1*/ NULL,
    /*0xb2*/ NULL,
    /*0xb3*/ NULL,
    /*0xb4*/ NULL,
    /*0xb5*/ NULL,
    /*0xb6*/ NULL,
    /*0xb7*/ NULL,
    /*0xb8*/ NULL,
    /*0xb9*/ NULL,
    /*0xba*/ &TaintEngine::Ext0FBA_Handler,
    /*0xbb*/ NULL,
    /*0xbc*/ NULL,
    /*0xbd*/ NULL,
    /*0xbe*/ NULL,
    /*0xbf*/ NULL,
    /*0xc0*/ NULL,
    /*0xc1*/ NULL,
    /*0xc2*/ NULL,
    /*0xc3*/ NULL,
    /*0xc4*/ NULL,
    /*0xc5*/ NULL,
    /*0xc6*/ NULL,
    /*0xc7*/ NULL,
    /*0xc8*/ NULL,
    /*0xc9*/ NULL,
    /*0xca*/ NULL,
    /*0xcb*/ NULL,
    /*0xcc*/ NULL,
    /*0xcd*/ NULL,
    /*0xce*/ NULL,
    /*0xcf*/ NULL,
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
    /*0xef*/ NULL,
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
        /* 7 */ &TaintEngine::Cmp_Handler,
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
        /* 7 */ &TaintEngine::Cmp_Handler,
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
        /* 7 */ &TaintEngine::Cmp_Handler,
    };
    TaintInstHandler h = handlers[MASK_MODRM_REG(inst->Aux.modrm)];
    if (h) {
        (this->*h)(cpu, inst);
    }
}

void TaintEngine::Ext8F_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtC0_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtC1_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtC6_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtC7_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtD0_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtD1_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtD2_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtD3_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtF6_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::ExtF7_Handler(const Processor *cpu, const Instruction *inst)
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

void TaintEngine::Ext0FBA_Handler(const Processor *cpu, const Instruction *inst)
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


DEFINE_CUSTOM_BINOP_HANDLER(Or_Handler,         TaintPropagate_Or)
DEFINE_CUSTOM_BINOP_HANDLER(Adc_Sbb_Handler,    TaintPropagate_Adc_Sbb)
DEFINE_CUSTOM_BINOP_HANDLER(And_Handler,        TaintPropagate_And)
DEFINE_CUSTOM_BINOP_HANDLER(Xor_Handler,        TaintPropagate_Xor)
DEFINE_CUSTOM_BINOP_HANDLER(Cmp_Handler,        TaintPropagate_Cmp)
DEFINE_CUSTOM_BINOP_HANDLER(Inc_Dec_Handler,    TaintPropagate_Inc_Dec)
