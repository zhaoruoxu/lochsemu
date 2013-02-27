#pragma once
 
#ifndef __TAINT_ENGINE_H__
#define __TAINT_ENGINE_H__
 
#include "Arietis.h"
#include "taint.h"
#include "instruction.h"
#include "parallel.h"
#include "instcontext.h"

struct ProcessorTaint {
    Taint GPRegs[8];
    Taint Flags[InstContext::FLAG_COUNT];
    Taint Eip;
};

class MemoryTaint {

    class PageTaint { 
    public:
        PageTaint();
        ~PageTaint();

        Taint   Get(u32 offset);
        void    Set(u32 offset, const Taint &t);
    private:
        //TaintFactory &  m_factory;
        Taint           m_data[LX_PAGE_SIZE];
    };


public:
    MemoryTaint();
    ~MemoryTaint();

    Taint       Get(u32 addr);
    Taint       Get(u32 addr, u32 len);     // 'OR' these Taint value
    void        Set(const Taint &t, u32 addr, u32 len = 1);

private:
    PageTaint *  GetPage(u32 addr);

private:
    //std::unordered_map<u32, Taint> m_status;
    PageTaint *  m_pagetable[LX_PAGE_COUNT];
    //TaintFactory &  m_factory;
};

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

    void        DefaultTaintPropagate   (Processor *cpu, const Instruction *inst);

    void        Enable(bool isEnabled);
    bool        IsEnabled() const;

private:
    Taint       GetTaint(const Processor *cpu, const ARGTYPE &oper);
    void        SetTaint(const Processor *cpu, const ARGTYPE &oper, const Taint &t);

    void        TaintIntroduce();
    void        TaintPropagate();   // !!! special handlers for instructions
    void        TaintSanitize();

private:
    typedef     void (TaintEngine::*TaintInstHandler)(const Processor *cpu, const Instruction *inst);
    static      TaintInstHandler HandlerOneByte[];
    static      TaintInstHandler HandlerTwoBytes[];

#define DECLARE_HANDLER(x)  void    x(const Processor *cpu, const Instruction *inst);

    DECLARE_HANDLER(DebugTaintIntroduce);

private:
    AEngine *   m_engine;
    Archive *   m_archive;
};
 
#endif // __TAINT_ENGINE_H__