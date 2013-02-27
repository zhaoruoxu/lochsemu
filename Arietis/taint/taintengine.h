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
    Taint       GetTaint8(const Processor *cpu, const ARGTYPE &oper);
    Taint16     GetTaint16(const Processor *cpu, const ARGTYPE &oper);
    Taint32     GetTaint32(const Processor *cpu, const ARGTYPE &oper);
    void        SetTaint8(const Processor *cpu, const ARGTYPE &oper, const Taint &t);
    void        SetTaint16(const Processor *cpu, const ARGTYPE &oper, const Taint16 &t);
    void        SetTaint32(const Processor *cpu, const ARGTYPE &oper, const Taint32 &t);

    void        TaintIntroduce();
    void        TaintPropagate();   // !!! special handlers for instructions
    void        TaintSanitize();

private:
    typedef     void (TaintEngine::*TaintInstHandler)(const Processor *cpu, const Instruction *inst);
    static      TaintInstHandler HandlerOneByte[];
    static      TaintInstHandler HandlerTwoBytes[];

#define DECLARE_HANDLER(x)  void    x(const Processor *cpu, const Instruction *inst);

    DECLARE_HANDLER(DebugTaintIntroduce);
    DECLARE_HANDLER(DefaultBinaryInst);

private:
    AEngine *   m_engine;
    Archive *   m_archive;
};
 
#endif // __TAINT_ENGINE_H__