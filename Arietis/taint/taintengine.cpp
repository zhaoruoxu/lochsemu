#include "stdafx.h"
#include "taintengine.h"
#include "processor.h"
#include "engine.h"

/*
 REG0 = 0x1,    -> 0
 REG1 = 0x2,    -> 1
 REG2 = 0x4,    -> 2
 REG3 = 0x8,    -> 3
 REG4 = 0x10,   -> 4
 REG5 = 0x20,   -> 5
 REG6 = 0x40,   -> 6
 REG7 = 0x80,   -> 7

 REG0 | REG2    -> 0        // TODO
 */

// static int RegMap[]  = {
//     -1,  0,  1, -1,  2,  0, -1, -1,  3, -1, -1, -1, -1, -1, -1, -1,
//      4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//      5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//      6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
//      7
// };


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


    //#define  DEBUG_TAINT    0
//     const ARGTYPE &arg1 = inst->Main.Argument1;
//     const ARGTYPE &arg2 = inst->Main.Argument2;
//     if (arg1.ArgType == NO_ARGUMENT || arg2.ArgType == NO_ARGUMENT ||
//         OPERAND_TYPE(arg2.ArgType) == CONSTANT_TYPE ||
//         inst->Main.Argument3.ArgType != NO_ARGUMENT) return;
// 
//     if (INST_TYPE(inst->Main.Inst.Category) != ARITHMETIC_INSTRUCTION)
//         return;

    //if (strstr(inst->Main.Inst.Mnemonic, "cmp")) return;

    // Taint introduce
    TaintInstHandler h;
    if (INST_ONEBYTE(inst->Main.Inst.Opcode)) {
        h = HandlerOneByte[inst->Main.Inst.Opcode];
    } else if (INST_TWOBYTE(inst->Main.Inst.Opcode)) {
        h = HandlerTwoBytes[inst->Main.Inst.Opcode];
    } else {
        Assert(0);
    }

    if (NULL != h) {
        (this->*h)(cpu, inst);
    }

    //DefaultTaintPropagate(cpu, inst);
//     if (strstr(inst->Main.Inst.Mnemonic, "add")) {
//         Taint t;
//         t.Set(DEBUG_TAINT);
//         SetTaint(cpu, arg1, t);
//     }
}


// Taint TaintEngine::GetTaint( const Processor *cpu, const ARGTYPE &oper )
// {
//     if (OPERAND_TYPE(oper.ArgType) == REGISTER_TYPE) {
//         int index = RegMap[REG_NUM(oper.ArgType)];
//         return CpuTaint.GPRegs[index];
//     } else if (OPERAND_TYPE(oper.ArgType) == MEMORY_TYPE) {
//         u32 o = cpu->Offset32(oper);
//         return MemTaint.Get(o, oper.ArgSize / 8);
//     } else {
//         return Taint();
//     }
// }
// 
// void TaintEngine::SetTaint( const Processor *cpu, const ARGTYPE &oper, const Taint &t )
// {
//     //Assert(t.GetIndices() > 0);
//     if (OPERAND_TYPE(oper.ArgType) == REGISTER_TYPE) {
//         CpuTaint.GPRegs[RegMap[REG_NUM(oper.ArgType)]] = t;
//     } else if (OPERAND_TYPE(oper.ArgType) == MEMORY_TYPE) {
//         u32 o = cpu->Offset32(oper);
//         return MemTaint.Set(t, o, oper.ArgSize / 8);
//     } else {
// 
//     }
// }
// 
// 
// void TaintEngine::DefaultTaintPropagate( Processor *cpu, const Instruction *inst )
// {
//     // op1 = op1 op op2
//     const ARGTYPE &arg1 = inst->Main.Argument1;
//     const ARGTYPE &arg2 = inst->Main.Argument2;
// 
//     if (OPERAND_TYPE(arg2.ArgType) == CONSTANT_TYPE) return;
//     Taint t = GetTaint(cpu, arg1) | GetTaint(cpu, arg2);
//     SetTaint(cpu, arg1, t);
// }

// static int TranslateReg(const ARGTYPE &oper)
// {
//     int index = RegMap[REG_NUM(oper.ArgType)];
//     Assert(index != -1);
//     return index;
// }
// 
// Taint TaintEngine::GetTaint1( const Processor *cpu, const ARGTYPE &oper )
// {
//     Assert(oper.ArgSize == 8);
//     if (IsRegArg(oper)) {
//         int index = TranslateReg(oper);
//         return CpuTaint.GPRegs[index].T[oper.ArgPosition / 8];
//     } else if (IsMemoryArg(oper)) {
//         // TODO: 需要考虑寻址寄存器的Taint和内存值的Taint
//         u32 o = cpu->Offset32(oper);
//         return MemTaint.Get1(o);
//     } else if (IsConstantArg(oper)) {
//         return Taint();
//     } else {
//         // no argument
//         Assert(0);
//         return Taint();
//     }
// }
// 
// 
// Taint2 TaintEngine::GetTaint2( const Processor *cpu, const ARGTYPE &oper )
// {
//     Assert(oper.ArgSize == 16);
// 
//     if (IsRegArg(oper)) {
//         int index = TranslateReg(oper);
//         return ToTaint2(CpuTaint.GPRegs[index], oper.ArgPosition / 8);
//     } else if (IsMemoryArg(oper)) {
//         // TODO: 需要考虑寻址寄存器的Taint和内存值的Taint
//         u32 o = cpu->Offset32(oper);
//         return MemTaint.Get2(o);
//     } else if (IsConstantArg(oper)) {
//         return Taint2();
//     } else {
//         Assert(0);
//         return Taint2();
//     }
// }
// 
// Taint4 TaintEngine::GetTaint4( const Processor *cpu, const ARGTYPE &oper )
// {
//     Assert(oper.ArgSize == 32);
// 
//     if (IsRegArg(oper)) {
//         int index = TranslateReg(oper);
//         return CpuTaint.GPRegs[index];
//     } else if (IsMemoryArg(oper)) {
//         // TODO: 需要考虑寻址寄存器的Taint和内存值的Taint
//         u32 o = cpu->Offset32(oper);
//         return MemTaint.Get4(o);
//     } else if (IsConstantArg(oper)) {
//         return Taint4();
//     } else {
//         Assert(0);
//         return Taint4();
//     }
// }
// 
// void TaintEngine::SetTaint1( const Processor *cpu, const ARGTYPE &oper, const Taint &t )
// {
//     Assert(oper.ArgSize == 8);
// 
//     if (IsRegArg(oper)) {
//         int index = TranslateReg(oper);
//         CpuTaint.GPRegs[index].T[oper.ArgPosition / 8] = t;
//     } else if (IsMemoryArg(oper)) {
//         // TODO: Save_rule
//         u32 o = cpu->Offset32(oper);
//         MemTaint.Set1(o, t);
//     }
//     Assert(0);
// }
// 
// void TaintEngine::SetTaint2( const Processor *cpu, const ARGTYPE &oper, const Taint2 &t )
// {
//     Assert(oper.ArgSize == 16);
//     if (IsRegArg(oper)) {
//         int index = TranslateReg(oper);
//         FromTaint2(CpuTaint.GPRegs[index], t, oper.ArgPosition / 8);
//     } else if (IsMemoryArg(oper)) {
//         // TODO: Save_rule
//         u32 o = cpu->Offset32(oper);
//         MemTaint.Set2(o, t);
//     }
//     Assert(0);
// }
// 
// void TaintEngine::SetTaint4( const Processor *cpu, const ARGTYPE &oper, const Taint4 &t )
// {
//     Assert(oper.ArgSize == 32);
//     if (IsRegArg(oper)) {
//         int index = TranslateReg(oper);
//         CpuTaint.GPRegs[index] = t;
//     } else if (IsMemoryArg(oper)) {
//         // TODO: Save_rule
//         u32 o = cpu->Offset32(oper);
//         MemTaint.Set4(o, t);
//     }
//     Assert(0);
// }

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
    //SetTaint(cpu, inst->Main.Argument1, t);
    Taint4 t;
    t.SetAll();
    TaintPropagate(cpu, inst, t);
}

// 
// void TaintEngine::SetFlagTaint1( const Instruction *inst, const Taint &t )
// {
//     u8p pFlag = (u8p) &inst->Main.Inst.Flags;
//     for (int i = 0; i < InstContext::FlagCount; i++) {
//         if (IsFlagModified(inst, i)) {
//             // Set flag's Taint to 't'
//             CpuTaint.Flags[i] = t;
//         } else if (IsFlagSet(inst, i) || IsFlagReset(inst, i)) {
//             // flag's Taint is sanitized, set to all '0'
//             CpuTaint.Flags[i].ResetAll();
//         }
//     }
// }
// 
// void TaintEngine::SetFlagTaint2( const Instruction *inst, const Taint2 &t )
// {
//     SetFlagTaint1(inst, Shrink(t));
// }
// 
// void TaintEngine::SetFlagTaint4( const Instruction *inst, const Taint4 &t )
// {
//     SetFlagTaint1(inst, Shrink(t));
// }

void TaintEngine::DefaultBinopHandler(const Processor *cpu, const Instruction *inst)
{
    Assert(ARG1.ArgSize == ARG2.ArgSize);

    if (ARG1.ArgSize == 32) {
        TaintPropagate_Binop<4>(cpu, inst);
    } else if (ARG1.ArgSize == 8) {
        TaintPropagate_Binop<1>(cpu, inst);
    } else if (ARG1.ArgSize == 16) {
        TaintPropagate_Binop<2>(cpu, inst);
    }


//     if (ARG1.ArgSize == 32) {
//         Taint4 t = TaintRule_Binop4(GetTaint4(cpu, ARG1), GetTaint4(cpu, ARG2));
//         SetTaint4(cpu, ARG1, t);
//         SetFlagTaint4(inst, t);
//     } else if (ARG1.ArgSize == 8) {
//         Taint  t = TaintRule_Binop1(GetTaint1(cpu, ARG1), GetTaint1(cpu, ARG2));
//         SetTaint1(cpu, ARG1, t);
//         SetFlagTaint1(inst, t);
//     } else if (ARG1.ArgSize == 16) {
//         Taint2 t = TaintRule_Binop2(GetTaint2(cpu, ARG1), GetTaint2(cpu, ARG2));
//         SetTaint2(cpu, ARG1, t);
//         SetFlagTaint2(inst, t);
//     } 
}


void TaintEngine::Add(const Processor *cpu, const Instruction *inst)
{

}


TaintEngine::TaintInstHandler TaintEngine::HandlerOneByte[] = {
    /*0x00*/ NULL,
    /*0x01*/ NULL,
    /*0x02*/ NULL,
    /*0x03*/ &TaintEngine::DefaultBinopHandler,
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
    /*0x1f*/ NULL,
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
    /*0x1f*/ NULL,
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
    /*0xb8*/ NULL,
    /*0xb9*/ NULL,
    /*0xba*/ NULL,
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
