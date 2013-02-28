#pragma once
 
#ifndef __TAINT_ENGINE_H__
#define __TAINT_ENGINE_H__
 
#include "Arietis.h"
#include "taint.h"
#include "instruction.h"
#include "parallel.h"
#include "instcontext.h"
#include "comptaint.h"

#include "processor.h"

#define ARG1    (inst->Main.Argument1)
#define ARG2    (inst->Main.Argument2)
#define ARG3    (inst->Main.Argument3)

class TaintEngine : public MutexSyncObject {
public:

    TaintEngine(AEngine *engine);
    ~TaintEngine();

    ProcessorTaint  CpuTaint;
    MemoryTaint     MemTaint;

    void        Initialize();
    void        OnPreExecute       (Processor *cpu, const Instruction *inst);
    void        OnPostExecute      (Processor *cpu, const Instruction *inst);
    void        OnWinapiPreCall    (Processor *cpu, uint apiIndex);
    void        OnWinapiPostCall   (Processor *cpu, uint apiIndex);

    void        UpdateInstContext(InstContext *ctx) const;

    //void        DefaultTaintPropagate   (Processor *cpu, const Instruction *inst);

    void        Enable(bool isEnabled);
    bool        IsEnabled() const;

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
        return MemTaint.Get<N>(o);
    }

    template <int N>
    void        TaintRule_Save(const Processor *cpu, const ARGTYPE &oper, const Tb<N> &t)
    {
        u32 o = cpu->Offset32(oper);
        MemTaint.Set(o, t);
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
            return FromTaint<4, N>(CpuTaint.GPRegs[index], oper.ArgPosition / 8);
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
            ToTaint(CpuTaint.GPRegs[index], t, oper.ArgPosition / 8);
        } else if (IsMemoryArg(oper)) {
            TaintRule_Save(cpu, oper, t);
        } else {
            Assert(0);
        }
    }


    template <int N>
    void        SetFlagTaint(const Instruction *inst, const Tb<N> &t)
    {
        for (int i = 0; i < InstContext::FlagCount; i++) {
            if (IsFlagModified(inst, i)) {
                CpuTaint.Flags[i] = Shrink(t);
            } else if (IsFlagSet(inst, i) || IsFlagReset(inst, i)) {
                CpuTaint.Flags[i].ResetAll();
            }
        }
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
        SetFlagTaint(inst, t);
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
                t.T[i].ResetAll();
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
                t.T[i].ResetAll();
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
        Taint1 cf   = CpuTaint.Flags[InstContext::CF];
        Tb<N> t1    = TaintRule_Binop(t0, Extend<N>(cf));
        TaintPropagate(cpu, inst, t1);
    }


private:
    typedef     void (TaintEngine::*TaintInstHandler)(const Processor *cpu, const Instruction *inst);
    static      TaintInstHandler HandlerOneByte[];
    static      TaintInstHandler HandlerTwoBytes[];

#define DECLARE_HANDLER(x)  void    x(const Processor *cpu, const Instruction *inst);

    DECLARE_HANDLER(DebugTaintIntroduce);
    DECLARE_HANDLER(DefaultBinopHandler);

    DECLARE_HANDLER(Ext80_Handler);
    DECLARE_HANDLER(Ext81_Handler);
    DECLARE_HANDLER(Ext83_Handler);

    DECLARE_HANDLER(Or_Handler);
    DECLARE_HANDLER(Adc_Sbb_Handler);
    DECLARE_HANDLER(And_Handler);
    DECLARE_HANDLER(Xor_Handler);

private:
    AEngine *   m_engine;
    Archive *   m_archive;
};

#endif // __TAINT_ENGINE_H__