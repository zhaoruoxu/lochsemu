#pragma once
 
#ifndef __TAINT_ENGINE_H__
#define __TAINT_ENGINE_H__
 
#include "Arietis.h"
#include "taint.h"
#include "instruction.h"

struct ProcessorTaint {

    Taint GPRegs[4 * 8];
    
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
public:
    MemoryTaint();
    Taint*      Get(u32 addr);
private:
    std::unordered_map<u32, Taint> m_status;
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

private:
    ProcessorTaint  m_cpuTaint;
    MemoryTaint     m_memTaint;

private:
    Taint*      GetTaint(const Processor *cpu, const Instruction *inst, const ARGTYPE &oper, int offset);

    void        TaintIntroduce();
    void        TaintPropagate();   // !!! special handlers for instructions
    void        TaintSanitize();

};
 
#endif // __TAINT_ENGINE_H__