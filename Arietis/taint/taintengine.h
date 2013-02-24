#pragma once
 
#ifndef __TAINT_ENGINE_H__
#define __TAINT_ENGINE_H__
 
#include "Arietis.h"
#include "taint.h"
#include "instruction.h"
#include "parallel.h"

struct ProcessorTaint {

    Taint GPRegs[8];
    
    Taint OF;
    Taint SF;
    Taint ZF;
    Taint AF;
    Taint PF;
    Taint CF;
    Taint TF;
    Taint IF;
    Taint DF;
    Taint NT;
    Taint RF;

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
    Mutex           m_mutex;
};

class TaintEngine {
public:

    TaintEngine();
    ~TaintEngine();

    void        Initialize();
    void        OnPreExecute       (Processor *cpu, const Instruction *inst);
    void        OnPostExecute      (Processor *cpu, const Instruction *inst);
    void        OnWinapiPreCall    (Processor *cpu, uint apiIndex);
    void        OnWinapiPostCall   (Processor *cpu, uint apiIndex);

    void        DefaultTaintPropagate   (Processor *cpu, const Instruction *inst);
private:
    ProcessorTaint  m_cpuTaint;
    MemoryTaint     m_memTaint;
    //TaintFactory    m_factory;

private:
    Taint       GetTaint(const Processor *cpu, const ARGTYPE &oper);
    void        SetTaint(const Processor *cpu, const ARGTYPE &oper, const Taint &t);

    void        TaintIntroduce();
    void        TaintPropagate();   // !!! special handlers for instructions
    void        TaintSanitize();

};
 
#endif // __TAINT_ENGINE_H__