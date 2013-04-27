#pragma once
 
#ifndef __TAINT_ENGINE_H__
#define __TAINT_ENGINE_H__
 
#include "prophet.h"
#include "taint.h"
#include "instruction.h"
#include "parallel.h"
#include "instcontext.h"
#include "comptaint.h"
#include "utilities.h"

#include "processor.h"
#include "process.h"
#include "thread.h"

#define ARG1    (inst->Main.Argument1)
#define ARG2    (inst->Main.Argument2)
#define ARG3    (inst->Main.Argument3)

class TaintEngine : public MutexSyncObject, public ISerializable {
public:
    static const int MaxCpus = Process::MaximumThreads;
public:

    TaintEngine(ProEngine *engine);
    ~TaintEngine();

    ProcessorTaint  CpuTaint[MaxCpus];
    MemoryTaint     MemTaint;

    void        Initialize();
    void        OnPreExecute       (PreExecuteEvent     &event);
    void        OnPostExecute      (PostExecuteEvent    &event);
    void        OnWinapiPreCall    (WinapiPreCallEvent  &event);
    void        OnWinapiPostCall   (WinapiPostCallEvent &event);

    void        UpdateInstContext(const Processor *cpu, InstContext *ctx) const;

    void        Enable(bool isEnabled);
    bool        IsEnabled() const { return m_enabled; }
    void        Reset();

    Taint1      GetTaintAddressingReg(const Processor *cpu, const ARGTYPE &oper) const;
    Taint1      GetTaintShrink(const Processor *cpu, const ARGTYPE &oper);
    Taint1      GetTestedFlagTaint(const Processor *cpu, const Instruction *inst) const;
    void        TaintMemoryRanged(u32 addr, u32 len, bool taintAllBits);

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

private:


    template <int N>
    Tb<N>       TaintRule_Binop(const Tb<N> &t1, const Tb<N> &t2)
    {
        return t1 | t2;
    }

    template <int N>
    Tb<N>       TaintRule_Load(const Processor *cpu, const ARGTYPE &oper)
    {
        u32 o = cpu->Offset32(oper);
        Taint1 t = GetTaintAddressingReg(cpu, oper);
        Tb<N>  s = Extend<N>(t);
        Tb<N>  m = MemTaint.Get<N>(o);
        return m  | s ;
    }

    template <int N>
    void        TaintRule_Save(const Processor *cpu, const ARGTYPE &oper, const Tb<N> &t)
    {
        u32 o = cpu->Offset32(oper);
        Taint1 r = GetTaintAddressingReg(cpu, oper);
        Tb<N>  s = Extend<N>(r);
        MemTaint.Set(o, t  | s );
    }

    void        TaintRule_ConditionalEip(const Processor *cpu, const Taint1 &t)
    {
        CpuTaint[cpu->IntID].Eip = t;
    }

    template <int N>
    Tb<N>       GetTaint(const Processor *cpu, const ARGTYPE &oper)
    {
        if (IsConstantArg(oper)) {
            return Tb<N>();
        }

        Assert(oper.ArgSize == N * 8);
        if (IsRegArg(oper)) {
            int index = TranslateReg(oper);
            return FromTaint<4, N>(CpuTaint[cpu->IntID].GPRegs[index], oper.ArgPosition / 8);
        } else if (IsMemoryArg(oper)) {
            return TaintRule_Load<N>(cpu, oper);
        } else {
            Assert(0);
            return Tb<N>();
        }
    }

    template <int N>
    void        SetTaint(const Processor *cpu, const ARGTYPE &oper, const Tb<N> &t)
    {
        Assert(oper.ArgSize == N * 8);
        if (IsRegArg(oper)) {
            int index = TranslateReg(oper);
            ToTaint(CpuTaint[cpu->IntID].GPRegs[index], t, oper.ArgPosition / 8);
        } else if (IsMemoryArg(oper)) {
            TaintRule_Save(cpu, oper, t);
        } else {
            Assert(0);
        }
    }

    Taint8      GetTaint8 (const Processor *cpu, const ARGTYPE &oper);
    Taint16     GetTaint16(const Processor *cpu, const ARGTYPE &oper);
    void        SetTaint8 (const Processor *cpu, const ARGTYPE &oper, const Taint8 &t);
    void        SetTaint16(const Processor *cpu, const ARGTYPE &oper, const Taint16 &t);

    template <int N>
    void        SetFlagTaint(const Processor *cpu, const Instruction *inst, const Tb<N> &t)
    {
        for (int i = 0; i < InstContext::FlagCount; i++) {
            if (IsFlagModified(inst, i)) {
                CpuTaint[cpu->IntID].Flags[i] = Shrink(t);
            } else if (IsFlagSet(inst, i) || IsFlagReset(inst, i)) {
                CpuTaint[cpu->IntID].Flags[i].ResetAll();
            }
        }
    }

    template <int X>
    Taint1      GetTestedFlagTaint1(const Processor *cpu) const
    {
        return CpuTaint[cpu->IntID].Flags[X];
    }

    template <int X, int Y>
    Taint1      GetTestedFlagTaint2(const Processor *cpu) const
    {
        return CpuTaint[cpu->IntID].Flags[X] | CpuTaint[cpu->IntID].Flags[Y];
    }

    template <int X, int Y, int Z>
    Taint1      GetTestedFlagTaint3(const Processor *cpu) const
    {
        return CpuTaint[cpu->IntID].Flags[X] | CpuTaint[cpu->IntID].Flags[Y] 
        | CpuTaint[cpu->IntID].Flags[Z];
    }

    Taint1      GetTestedEcxTaint(const Processor *cpu) const
    {
        return Shrink(CpuTaint[cpu->IntID].GPRegs[LX_REG_ECX]);
    }

    template <int N>
    void        TaintPropagate_Binop(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t     = TaintRule_Binop(GetTaint<N>(cpu, ARG1), GetTaint<N>(cpu, ARG2));
        TaintPropagate(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate(const Processor *cpu, const Instruction *inst, const Tb<N> &t)
    {
        SetTaint(cpu, ARG1, t);
        SetFlagTaint(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate_Or(const Processor *cpu, const Instruction *inst)
    {
        if (!IsConstantArg(ARG2)) {
            TaintPropagate_Binop<N>(cpu, inst);
            return;
        }

        Tb<N> t     = GetTaint<N>(cpu, ARG1);
        u64 val     = inst->Main.Inst.Immediat;
        if (ARG2.ArgSize == 8) 
            val     = SIGN_EXTEND(8, 64, val);
        u8p pimm    = (u8p) &val;
        for (int i = 0; i < N; i++) {
            if (pimm[i] == 0xff) {   // x OR 0xff is always 0xff : Taint is sanitized
                t[i].ResetAll();
            } else {
                // binop rule?
            }
        }
        TaintPropagate(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate_And(const Processor *cpu, const Instruction *inst)
    {
        if (!IsConstantArg(ARG2)) {
            TaintPropagate_Binop<N>(cpu, inst);
            return;
        }

        Tb<N> t     = GetTaint<N>(cpu, ARG1);
        u64 val     = inst->Main.Inst.Immediat;
        if (ARG2.ArgSize == 8)
            val     = SIGN_EXTEND(8, 64, val);
        u8p pimm    = (u8p) &val;
        for (int i = 0; i < N; i++) {
            if (pimm[i] == 0x00) {   // x AND 0x00 is always 0 : Taint is sanitized
                t[i].ResetAll();
            } else {
                // binop rule?
            }
        }
        TaintPropagate(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate_Xor(const Processor *cpu, const Instruction *inst)
    {
        if (ARG1.ArgType == ARG2.ArgType) {
            // xor r0, r0
            Tb<N> t;
            t.ResetAll();
            TaintPropagate(cpu, inst, t);
        } else {
            TaintPropagate_Binop<N>(cpu, inst);
        }
    }

    template <int N>
    void        TaintPropagate_Adc_Sbb(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t0    = TaintRule_Binop(GetTaint<N>(cpu, ARG1), GetTaint<N>(cpu, ARG2));
        Taint1 cf   = CpuTaint[cpu->IntID].Flags[InstContext::CF];
        Tb<N> t1    = TaintRule_Binop(t0, Extend<N>(cf));
        TaintPropagate(cpu, inst, t1);
    }

    template <int N>
    void        TaintPropagate_Cmp_Test(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t     = TaintRule_Binop(GetTaint<N>(cpu, ARG1), GetTaint<N>(cpu, ARG2));
        SetFlagTaint(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate_Inc_Dec(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t     = GetTaint<N>(cpu, ARG1);
        SetFlagTaint(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate_Push(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t     = GetTaint<N>(cpu, ARG2);
        MemTaint.Set(cpu->ESP, t);  // should esp be tainted?
    }

    template <int N>
    void        TaintPropagate_Pop(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t     = MemTaint.Get<N>(cpu->ESP - N);
        SetTaint(cpu, ARG1, t);
    }

    template <int N>
    void        TaintPropagate_Imul(const Processor *cpu, const Instruction *inst)
    {
        if (inst->Main.Inst.Immediat == 0) {
            TaintPropagate(cpu, inst, Tb<N>());
        } else {
            TaintPropagate_Binop<N>(cpu, inst);
        }
    }

    void        TaintPropagate_CJmp(const Processor *cpu, const Instruction *inst)
    {
        TaintRule_ConditionalEip(cpu, GetTestedFlagTaint(cpu, inst));
    }

    void        TaintPropagate_Setcc(const Processor *cpu, const Instruction *inst)
    {
        SetTaint<1>(cpu, ARG2, GetTestedFlagTaint(cpu, inst));
    }

    void        TaintPropagate_Cmovcc(const Processor *cpu, const Instruction *inst)
    {
        if (cpu->IsJumpTaken(inst))
            Mov_Handler(cpu, inst);
    }

    template <int N>
    void        TaintPropagate_Xchg(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t1    = GetTaint<N>(cpu, ARG1);
        Tb<N> t2    = GetTaint<N>(cpu, ARG2);
        SetTaint<N>(cpu, ARG1, t2);
        SetTaint<N>(cpu, ARG2, t1);
    }

    template <int N>
    void        TaintPropagate_Mov(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t;
        if (!IsConstantArg(ARG2)) {
            t       = GetTaint<N>(cpu, ARG2);
        }
        SetTaint<N>(cpu, ARG1, t);
    }

    template <int N>
    void        TaintPropagate_Lea(const Processor *cpu, const Instruction *inst)
    {
        Taint1 t    = GetTaintAddressingReg(cpu, ARG2);
        SetTaint(cpu, ARG1, Extend<N>(t));
    }

    template <int N>
    void        TaintPropagate_Movs(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t = MemTaint.Get<N>(cpu->DF == 0 ? cpu->ESI - N : cpu->ESI + N);
        MemTaint.Set<N>(cpu->DF == 0 ? cpu->ESI - N : cpu->ESI + N, t);
    }

    template <int N>
    void        TaintPropagate_Cmps(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t1 = MemTaint.Get<N>(cpu->DF == 0 ? cpu->ESI - N : cpu->ESI + N);
        Tb<N> t2 = MemTaint.Get<N>(cpu->DF == 0 ? cpu->EDI - N : cpu->ESI + N);
        Tb<N> t = TaintRule_Binop(t1, t2);
        SetFlagTaint(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate_Stos(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t = FromTaint<4, N>(CpuTaint[cpu->IntID].GPRegs[LX_REG_EAX], 0);
        MemTaint.Set<N>(cpu->DF == 0 ? cpu->EDI - N : cpu->EDI + N, t);
    }

    template <int N>
    void        TaintPropagate_Lods(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t = MemTaint.Get<N>(cpu->DF == 0 ? cpu->ESI - N : cpu->ESI + N);
        ToTaint<N, 4>(CpuTaint[cpu->IntID].GPRegs[LX_REG_EAX], t, 0);
    }

    template <int N>
    void        TaintPropagate_Scas(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t1 = FromTaint<4, N>(CpuTaint[cpu->IntID].GPRegs[LX_REG_EAX], 0);
        Tb<N> t2 = MemTaint.Get<N>(cpu->DF == 0 ? cpu->EDI - N : cpu->EDI + N);
        Tb<N> t = TaintRule_Binop(t1, t2);
        SetFlagTaint(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate_ShiftRotate(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t1 = GetTaint<N>(cpu, ARG1);
        Taint1 t = Shrink(t1);
        Tb<N> newT = Extend<N>(t);
        SetTaint(cpu, ARG1, newT);
        CpuTaint[cpu->IntID].Flags[InstContext::CF] = t;
        CpuTaint[cpu->IntID].Flags[InstContext::OF] = t;
    }

    template <int X>
    void        TaintPropagate_ClearFlag(const Processor *cpu, const Instruction *inst)
    {
        CpuTaint[cpu->IntID].Flags[X].ResetAll();
    }

    template <int N>
    void        TaintPropagate_ClearDest(const Processor *cpu, const Instruction *inst)
    {
        SetTaint<N>(cpu, ARG1, Tb<N>());
    }

    template <int N>
    void        TaintPropagate_Neg(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t = GetTaint<N>(cpu, ARG1);
        SetFlagTaint(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate_Bt(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t = TaintRule_Binop(GetTaint<N>(cpu, ARG1), GetTaint<N>(cpu, ARG2));
        SetTaint<N>(cpu, ARG1, t);
        CpuTaint[cpu->IntID].Flags[InstContext::CF] = Shrink<N>(t);
    }

    template <int N>
    void        TaintPropagate_Xadd(const Processor *cpu, const Instruction *inst)
    {
        Tb<N> t1    = GetTaint<N>(cpu, ARG1);
        Tb<N> t2    = GetTaint<N>(cpu, ARG2);
        Tb<N> t     = TaintRule_Binop(t1, t2);
        SetTaint(cpu, ARG1, t);
        SetTaint(cpu, ARG2, t1);
        SetFlagTaint(cpu, inst, t);
    }

private:
    typedef     void (TaintEngine::*TaintInstHandler)(const Processor *cpu, const Instruction *inst);
    static      TaintInstHandler HandlerOneByte[];
    static      TaintInstHandler HandlerTwoBytes[];

#define DECLARE_HANDLER(x)  void    x(const Processor *cpu, const Instruction *inst);

    DECLARE_HANDLER(DebugTaintIntroduce);
    DECLARE_HANDLER(DefaultBinopHandler);
    DECLARE_HANDLER(DefaultSIMDBinopHandler);

    DECLARE_HANDLER(Ext80_Handler);
    DECLARE_HANDLER(Ext81_Handler);
    DECLARE_HANDLER(Ext83_Handler);
    DECLARE_HANDLER(Ext8F_Handler);
    DECLARE_HANDLER(ExtC0_Handler);
    DECLARE_HANDLER(ExtC1_Handler);
    DECLARE_HANDLER(ExtC6_Handler);
    DECLARE_HANDLER(ExtC7_Handler);
    DECLARE_HANDLER(ExtD0_Handler);
    DECLARE_HANDLER(ExtD1_Handler);
    DECLARE_HANDLER(ExtD2_Handler);
    DECLARE_HANDLER(ExtD3_Handler);
    DECLARE_HANDLER(ExtF6_Handler);
    DECLARE_HANDLER(ExtF7_Handler);
    DECLARE_HANDLER(ExtFE_Handler);
    DECLARE_HANDLER(ExtFF_Handler);
    DECLARE_HANDLER(Ext0F1F_Handler);
    DECLARE_HANDLER(Ext0FAE_Handler);
    DECLARE_HANDLER(Ext0FBA_Handler);


    DECLARE_HANDLER(Or_Handler);
    DECLARE_HANDLER(Adc_Sbb_Handler);
    DECLARE_HANDLER(And_Handler);
    DECLARE_HANDLER(Xor_Handler);
    DECLARE_HANDLER(Cmp_Test_Handler);
    DECLARE_HANDLER(Inc_Dec_Handler);
    DECLARE_HANDLER(Push_Handler);
    DECLARE_HANDLER(Pop_Handler);
    DECLARE_HANDLER(Imul69_Imul6B_Handler);
    DECLARE_HANDLER(ImulF6_MulF6_Handler);
    DECLARE_HANDLER(ImulF7_MulF7_Handler);
    //DECLARE_HANDLER(LoopE2_JecxzE3_Handler);
    DECLARE_HANDLER(Xchg_Handler);
    DECLARE_HANDLER(Mov_Handler);
    DECLARE_HANDLER(Lea_Handler);
    DECLARE_HANDLER(Cbw_Handler);
    DECLARE_HANDLER(Cdq_Handler);
    DECLARE_HANDLER(Sahf_Handler);
    DECLARE_HANDLER(Movs_Handler);
    DECLARE_HANDLER(Cmps_Handler);
    DECLARE_HANDLER(Stos_Handler);
    DECLARE_HANDLER(Lods_Handler);
    DECLARE_HANDLER(Scas_Handler);
    DECLARE_HANDLER(ShiftRotate_Handler);
    DECLARE_HANDLER(Ret_Handler);
    DECLARE_HANDLER(CallAbs_Handler);
    DECLARE_HANDLER(CallRel_handler);
    DECLARE_HANDLER(JmpAbs_Handler);
    DECLARE_HANDLER(JmpRel_Handler);
    DECLARE_HANDLER(Neg_Handler);
    DECLARE_HANDLER(DivF7_IdivF7_Handler);
    DECLARE_HANDLER(Bt_Handler);
    DECLARE_HANDLER(Movapd660F28_Handler);
    DECLARE_HANDLER(Movdqa0F6F_7F_Handler);
    DECLARE_HANDLER(Movd0F6E_Handler);
    DECLARE_HANDLER(Movd0F7E_Handler);
    DECLARE_HANDLER(ClearEaxEdx_Handler);
    DECLARE_HANDLER(Cpuid_Handler);
    DECLARE_HANDLER(Shld_Shrd_Handler);
    DECLARE_HANDLER(Cmpxchg0FB1_Handler);
    DECLARE_HANDLER(Movzx_Handler);
    DECLARE_HANDLER(Movsx_Handler);
    DECLARE_HANDLER(Xadd_Handler);
    DECLARE_HANDLER(Bswap_Handler);
    DECLARE_HANDLER(Pxor0FEF_Handler);
    DECLARE_HANDLER(Punpckldq_Handler);
    DECLARE_HANDLER(Pshufw_Handler);
    DECLARE_HANDLER(Movq0FD6_Handler);

#undef DECLARE_HANDLER
private:
    ProEngine *   m_engine;
    bool        m_enabled;
};

#endif // __TAINT_ENGINE_H__