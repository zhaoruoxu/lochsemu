#pragma once
 
#ifndef __TAINT_ENGINE_H__
#define __TAINT_ENGINE_H__
 
#include "Arietis.h"
#include "taint.h"
#include "instruction.h"
#include "parallel.h"
#include "instcontext.h"
#include "comptaint.h"

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
//     Taint       GetTaint1(const Processor *cpu, const ARGTYPE &oper);
//     Taint2      GetTaint2(const Processor *cpu, const ARGTYPE &oper);
//     Taint4      GetTaint4(const Processor *cpu, const ARGTYPE &oper);
//     void        SetTaint1(const Processor *cpu, const ARGTYPE &oper, const Taint &t);
//     void        SetTaint2(const Processor *cpu, const ARGTYPE &oper, const Taint2 &t);
//     void        SetTaint4(const Processor *cpu, const ARGTYPE &oper, const Taint4 &t);
// 
//     void        SetFlagTaint1(const Instruction *inst, const Taint &t);
//     void        SetFlagTaint2(const Instruction *inst, const Taint2 &t);
//     void        SetFlagTaint4(const Instruction *inst, const Taint4 &t);

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
        Assert(oper.ArgSize == N * 8);
        if (IsRegArg(oper)) {
            int index = TranslateReg(oper);
            return FromTaint<4, N>(CpuTaint.GPRegs[index], oper.ArgPosition / 8);
        } else if (IsMemoryArg(oper)) {
            return TaintRule_Load<N>(cpu, oper);
        } else if (IsConstantArg(oper)) {
            return Tb<N>();
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
        Tb<N> t1 = GetTaint<N>(cpu, ARG1);
        Tb<N> t2 = GetTaint<N>(cpu, ARG2);
        Tb<N> t = TaintRule_Binop(t1, t2);
        TaintPropagate(cpu, inst, t);
    }

    template <int N>
    void        TaintPropagate(const Processor *cpu, const Instruction *inst, const Tb<N> &t)
    {
        SetTaint(cpu, ARG1, t);
        SetFlagTaint(inst, t);
    }
// 
//     void        TaintIntroduce();
//     void        TaintPropagate();   // !!! special handlers for instructions
//     void        TaintSanitize();

    // rule guided tainting
//     Taint       TaintRule_Binop1(const Taint  &t1, const Taint  &t2);
//     Taint2      TaintRule_Binop2(const Taint2 &t1, const Taint2 &t2);
//     Taint4      TaintRule_Binop4(const Taint4 &t1, const Taint4 &t2);




private:
    typedef     void (TaintEngine::*TaintInstHandler)(const Processor *cpu, const Instruction *inst);
    static      TaintInstHandler HandlerOneByte[];
    static      TaintInstHandler HandlerTwoBytes[];

#define DECLARE_HANDLER(x)  void    x(const Processor *cpu, const Instruction *inst);

    DECLARE_HANDLER(DebugTaintIntroduce);
    DECLARE_HANDLER(DefaultBinopHandler);

    DECLARE_HANDLER(Add);

private:
    AEngine *   m_engine;
    Archive *   m_archive;
};

#endif // __TAINT_ENGINE_H__