#pragma once
 
#ifndef __TAINT_ENGINE_H__
#define __TAINT_ENGINE_H__
 
#include "Arietis.h"
#include "taint.h"
#include "instruction.h"
#include "parallel.h"
#include "instcontext.h"
#include "comptaint.h"


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
    Taint       GetTaint1(const Processor *cpu, const ARGTYPE &oper);
    Taint2      GetTaint2(const Processor *cpu, const ARGTYPE &oper);
    Taint4      GetTaint4(const Processor *cpu, const ARGTYPE &oper);
    void        SetTaint1(const Processor *cpu, const ARGTYPE &oper, const Taint &t);
    void        SetTaint2(const Processor *cpu, const ARGTYPE &oper, const Taint2 &t);
    void        SetTaint4(const Processor *cpu, const ARGTYPE &oper, const Taint4 &t);

    void        SetFlagTaint1(const Instruction *inst, const Taint &t);
    void        SetFlagTaint2(const Instruction *inst, const Taint2 &t);
    void        SetFlagTaint4(const Instruction *inst, const Taint4 &t);

    void        TaintIntroduce();
    void        TaintPropagate();   // !!! special handlers for instructions
    void        TaintSanitize();

    // rule guided tainting
    Taint       TaintRule_Binop1(const Taint  &t1, const Taint  &t2);
    Taint2      TaintRule_Binop2(const Taint2 &t1, const Taint2 &t2);
    Taint4      TaintRule_Binop4(const Taint4 &t1, const Taint4 &t2);

    void        OverrideBinopHandler(const Processor *cpu, const Instruction *inst, )

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