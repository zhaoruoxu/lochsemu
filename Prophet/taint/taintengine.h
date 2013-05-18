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
#include "protocol/protocol.h"

#include "processor.h"
#include "process.h"
#include "thread.h"

#include "protocol/runtrace.h"

#define ARG1    (inst->Main.Argument1)
#define ARG2    (inst->Main.Argument2)
#define ARG3    (inst->Main.Argument3)

class TSnapshot {
    friend class TaintEngine;
public:
    TSnapshot(TaintEngine &t);
    virtual ~TSnapshot();

    void    Dump(File &f);

private:
    ProcessorTaint *m_pt;
    MemoryTaint *m_mt;
};

enum TaintRule {
    TAINT_LOADADDRREG = 1 << 0,
    TAINT_SAVEADDRREG = 1 << 1,
};

class TaintEngine : public ISerializable {
public:
    //static const int MaxCpus = Process::MaximumThreads;
public:

    TaintEngine(ProEngine *engine);
    ~TaintEngine();

    ProcessorTaint  CpuTaint;
    MemoryTaint     MemTaint;

    void        Initialize();
    void        OnPreExecute       (PreExecuteEvent     &event);
    void        OnPostExecute      (PostExecuteEvent    &event);
    void        OnWinapiPreCall    (WinapiPreCallEvent  &event);
    void        OnWinapiPostCall   (WinapiPostCallEvent &event);

    void        OnExecuteTrace      (ExecuteTraceEvent  &event);

    void        Enable(bool isEnabled);
    bool        IsEnabled() const { return m_enabled; }
    void        Reset();

    Taint1      GetTaintAddressingReg(const TContext *t, const ARGTYPE &oper) const;
    Taint1      GetTaintShrink(const TContext *t, const ARGTYPE &oper);
    Taint1      GetTestedFlagTaint(const TContext *t, const Instruction *inst) const;
    void        TaintMemoryRanged(u32 addr, u32 len, bool taintAllBits);

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

    void        ApplySnapshot(const TSnapshot &t);
    void        SetTaintRule(TaintRule r, bool enable);
    bool        TaintRuleEnabled(TaintRule r);
    void        TaintRule_LoadDefault();
    void        TaintRule_LoadMemory();

private:
    ProEngine *   m_engine;
    bool        m_enabled;
    u32         m_taintRule;
private:


    template <int N>
    Tb<N>       TaintRule_Binop(const Tb<N> &t1, const Tb<N> &t2)
    {
        return t1 | t2;
    }

    template <int N>
    Tb<N>       TaintRule_Load(const TContext *ctx, const ARGTYPE &oper)
    {
        u32 o = ctx->Mr.Addr;/*cpu->Offset32(oper);*/
        Tb<N>  m = MemTaint.Get<N>(o);
        if (TaintRuleEnabled(TAINT_LOADADDRREG)) {
            Taint1 t = GetTaintAddressingReg(ctx, oper);
            Tb<N>  s = Extend<N>(t);
            return m | s ;
        } else {
            return m;
        }
    }

    template <int N>
    void        TaintRule_Save(const TContext *ctx, const ARGTYPE &oper, const Tb<N> &t)
    {
        u32 o = ctx->Mw.Addr;/*cpu->Offset32(oper);*/
        if (TaintRuleEnabled(TAINT_SAVEADDRREG)) {
            Taint1 r = GetTaintAddressingReg(ctx, oper);
            Tb<N>  s = Extend<N>(r);
            MemTaint.Set(o, t | s );
        } else {
            MemTaint.Set(o, t);
        }
    }

    void        TaintRule_ConditionalEip(const TContext *ctx, const Taint1 &t)
    {
        CpuTaint.Eip = t;
    }

    template <int N>
    Tb<N>       GetTaint(const TContext *ctx, const ARGTYPE &oper)
    {
        if (IsConstantArg(oper)) {
            return Tb<N>();
        }

        Assert(oper.ArgSize == N * 8);
        if (IsRegArg(oper)) {
            int index = TranslateReg(oper);
            return FromTaint<4, N>(CpuTaint.GPRegs[index], oper.ArgPosition / 8);
        } else if (IsMemoryArg(oper)) {
            return TaintRule_Load<N>(ctx, oper);
        } else {
            Assert(0);
            return Tb<N>();
        }
    }

    template <int N>
    void        SetTaint(const TContext *ctx, const ARGTYPE &oper, const Tb<N> &t)
    {
        Assert(oper.ArgSize == N * 8);
        if (IsRegArg(oper)) {
            int index = TranslateReg(oper);
            ToTaint(CpuTaint.GPRegs[index], t, oper.ArgPosition / 8);
        } else if (IsMemoryArg(oper)) {
            TaintRule_Save(ctx, oper, t);
        } else {
            Assert(0);
        }
    }

    Taint8      GetTaint8 (const TContext *ctx, const ARGTYPE &oper);
    Taint16     GetTaint16(const TContext *ctx, const ARGTYPE &oper);
    void        SetTaint8 (const TContext *ctx, const ARGTYPE &oper, const Taint8 &t);
    void        SetTaint16(const TContext *ctx, const ARGTYPE &oper, const Taint16 &t);

    template <int N>
    void        SetFlagTaint(const TContext *ctx, const Instruction *inst, const Tb<N> &t)
    {
        for (int i = 0; i < InstContext::FlagCount; i++) {
            if (IsFlagModified(inst, i)) {
                CpuTaint.Flags[i] = Shrink(t);
            } else if (IsFlagSet(inst, i) || IsFlagReset(inst, i)) {
                CpuTaint.Flags[i].ResetAll();
            }
        }
    }

    template <int X>
    Taint1      GetTestedFlagTaint1(const TContext *ctx) const
    {
        return CpuTaint.Flags[X];
    }

    template <int X, int Y>
    Taint1      GetTestedFlagTaint2(const TContext *ctx) const
    {
        return CpuTaint.Flags[X] | CpuTaint.Flags[Y];
    }

    template <int X, int Y, int Z>
    Taint1      GetTestedFlagTaint3(const TContext *ctx) const
    {
        return CpuTaint.Flags[X] | CpuTaint.Flags[Y] | CpuTaint.Flags[Z];
    }

    Taint1      GetTestedEcxTaint(const TContext *ctx) const
    {
        return Shrink(CpuTaint.GPRegs[LX_REG_ECX]);
    }

    template <int N>
    void        TaintPropagate_Binop(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t     = TaintRule_Binop(GetTaint<N>(ctx, ARG1), GetTaint<N>(ctx, ARG2));
        TaintPropagate(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate(const TContext *ctx, const Instruction *inst, const Tb<N> &t)
    {
        SetTaint(ctx, ARG1, t);
        SetFlagTaint(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate_Or(const TContext *ctx, const Instruction *inst)
    {
        if (!IsConstantArg(ARG2)) {
            TaintPropagate_Binop<N>(ctx, inst);
            return;
        }

        Tb<N> t     = GetTaint<N>(ctx, ARG1);
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
        TaintPropagate(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate_And(const TContext *ctx, const Instruction *inst)
    {
        if (!IsConstantArg(ARG2)) {
            TaintPropagate_Binop<N>(ctx, inst);
            return;
        }

        Tb<N> t     = GetTaint<N>(ctx, ARG1);
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
        TaintPropagate(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate_Xor(const TContext *ctx, const Instruction *inst)
    {
        if (ARG1.ArgType == ARG2.ArgType) {
            // xor r0, r0
            Tb<N> t;
            t.ResetAll();
            TaintPropagate(ctx, inst, t);
        } else {
            TaintPropagate_Binop<N>(ctx, inst);
        }
    }

    template <int N>
    void        TaintPropagate_Adc_Sbb(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t0    = TaintRule_Binop(GetTaint<N>(ctx, ARG1), GetTaint<N>(ctx, ARG2));
        Taint1 cf   = CpuTaint.Flags[InstContext::CF];
        Tb<N> t1    = TaintRule_Binop(t0, Extend<N>(cf));
        TaintPropagate(ctx, inst, t1);
    }

    template <int N>
    void        TaintPropagate_Cmp_Test(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t     = TaintRule_Binop(GetTaint<N>(ctx, ARG1), GetTaint<N>(ctx, ARG2));
        SetFlagTaint(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate_Inc_Dec(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t     = GetTaint<N>(ctx, ARG1);
        SetFlagTaint(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate_Push(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t     = GetTaint<N>(ctx, ARG2);
        MemTaint.Set(ctx->Regs[LX_REG_ESP], t);  // should esp be tainted?
    }

    template <int N>
    void        TaintPropagate_Pop(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t     = MemTaint.Get<N>(ctx->Regs[LX_REG_ESP] - N);
        SetTaint(ctx, ARG1, t);
    }

    template <int N>
    void        TaintPropagate_Imul(const TContext *ctx, const Instruction *inst)
    {
        if (inst->Main.Inst.Immediat == 0) {
            TaintPropagate(ctx, inst, Tb<N>());
        } else {
            TaintPropagate_Binop<N>(ctx, inst);
        }
    }

    void        TaintPropagate_CJmp(const TContext *ctx, const Instruction *inst)
    {
        TaintRule_ConditionalEip(ctx, GetTestedFlagTaint(ctx, inst));
    }

    void        TaintPropagate_Setcc(const TContext *ctx, const Instruction *inst)
    {
        SetTaint<1>(ctx, ARG2, GetTestedFlagTaint(ctx, inst));
    }

    void        TaintPropagate_Cmovcc(const TContext *ctx, const Instruction *inst)
    {
        if (ctx->JumpTaken)
            Mov_Handler(ctx, inst);
    }

    template <int N>
    void        TaintPropagate_Xchg(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t1    = GetTaint<N>(ctx, ARG1);
        Tb<N> t2    = GetTaint<N>(ctx, ARG2);
        SetTaint<N>(ctx, ARG1, t2);
        SetTaint<N>(ctx, ARG2, t1);
    }

    template <int N>
    void        TaintPropagate_Mov(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t;
        if (!IsConstantArg(ARG2)) {
            t       = GetTaint<N>(ctx, ARG2);
        }
        SetTaint<N>(ctx, ARG1, t);
    }

    template <int N>
    void        TaintPropagate_Lea(const TContext *ctx, const Instruction *inst)
    {
        Taint1 t    = GetTaintAddressingReg(ctx, ARG2);
        SetTaint(ctx, ARG1, Extend<N>(t));
    }

    template <int N>
    void        TaintPropagate_Movs(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t = MemTaint.Get<N>(ctx->Flag(InstContext::DF) == 0 ? 
            ctx->Regs[LX_REG_ESI] - N : ctx->Regs[LX_REG_ESI] + N);
        MemTaint.Set<N>(ctx->Flag(InstContext::DF) == 0 ? 
            ctx->Regs[LX_REG_ESI] - N : ctx->Regs[LX_REG_ESI] + N, t);
    }

    template <int N>
    void        TaintPropagate_Cmps(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t1 = MemTaint.Get<N>(ctx->Flag(InstContext::DF) == 0 ? 
            ctx->Regs[LX_REG_ESI] - N : ctx->Regs[LX_REG_ESI] + N);
        Tb<N> t2 = MemTaint.Get<N>(ctx->Flag(InstContext::DF) == 0 ? 
            ctx->Regs[LX_REG_EDI] - N : ctx->Regs[LX_REG_EDI] + N);
        Tb<N> t = TaintRule_Binop(t1, t2);
        SetFlagTaint(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate_Stos(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t = FromTaint<4, N>(CpuTaint.GPRegs[LX_REG_EAX], 0);
        MemTaint.Set<N>(ctx->Flag(InstContext::DF) == 0 ? 
            ctx->Regs[LX_REG_EDI] - N : ctx->Regs[LX_REG_EDI] + N, t);
    }

    template <int N>
    void        TaintPropagate_Lods(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t = MemTaint.Get<N>(ctx->Flag(InstContext::DF) == 0 ? 
            ctx->Regs[LX_REG_ESI] - N : ctx->Regs[LX_REG_ESI] + N);
        ToTaint<N, 4>(CpuTaint.GPRegs[LX_REG_EAX], t, 0);
    }

    template <int N>
    void        TaintPropagate_Scas(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t1 = FromTaint<4, N>(CpuTaint.GPRegs[LX_REG_EAX], 0);
        Tb<N> t2 = MemTaint.Get<N>(ctx->Flag(InstContext::DF) == 0 ? 
            ctx->Regs[LX_REG_EDI] - N : ctx->Regs[LX_REG_EDI] + N);
        Tb<N> t = TaintRule_Binop(t1, t2);
        SetFlagTaint(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate_ShiftRotate(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t1 = GetTaint<N>(ctx, ARG1);
        Taint1 t = Shrink(t1);
        Tb<N> newT = Extend<N>(t);
        SetTaint(ctx, ARG1, newT);
        CpuTaint.Flags[InstContext::CF] = t;
        CpuTaint.Flags[InstContext::OF] = t;
    }

    template <int X>
    void        TaintPropagate_ClearFlag(const TContext *ctx, const Instruction *inst)
    {
        CpuTaint.Flags[X].ResetAll();
    }

    template <int N>
    void        TaintPropagate_ClearDest(const TContext *ctx, const Instruction *inst)
    {
        SetTaint<N>(ctx, ARG1, Tb<N>());
    }

    template <int N>
    void        TaintPropagate_Neg(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t = GetTaint<N>(ctx, ARG1);
        SetFlagTaint(ctx, inst, t);
    }

    template <int N>
    void        TaintPropagate_Bt(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t = TaintRule_Binop(GetTaint<N>(ctx, ARG1), GetTaint<N>(ctx, ARG2));
        SetTaint<N>(ctx, ARG1, t);
        CpuTaint.Flags[InstContext::CF] = Shrink<N>(t);
    }

    template <int N>
    void        TaintPropagate_Xadd(const TContext *ctx, const Instruction *inst)
    {
        Tb<N> t1    = GetTaint<N>(ctx, ARG1);
        Tb<N> t2    = GetTaint<N>(ctx, ARG2);
        Tb<N> t     = TaintRule_Binop(t1, t2);
        SetTaint(ctx, ARG1, t);
        SetTaint(ctx, ARG2, t1);
        SetFlagTaint(ctx, inst, t);
    }

private:
    typedef     void (TaintEngine::*TaintInstHandler)(const TContext *ctx, const Instruction *inst);
    static      TaintInstHandler HandlerOneByte[];
    static      TaintInstHandler HandlerTwoBytes[];

#define DECLARE_HANDLER(x)  void    x(const TContext *ctx, const Instruction *inst);

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

};

#endif // __TAINT_ENGINE_H__