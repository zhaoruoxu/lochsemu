#pragma once

#ifndef __ARIETIS_DEBUGGER_H__
#define __ARIETIS_DEBUGGER_H__

#include "Arietis.h"
#include "parallel.h"

class Debugger {
public:
    //typedef std::function<void (u32 addr)>                  PtrChangeHandler;
public:
    Debugger(ArietisEngine *engine);
    ~Debugger();

    void        Initialize();
    void        OnPreExecute(Processor *cpu, const Instruction *inst);
    void        OnPostExecute(Processor *cpu, const Instruction *inst);
    void        OnStepInto();

//     void        RegisterPtrChangeHandler(PtrChangeHandler h) { 
//         m_ptrChangeHandler = h; 
//     }

private:
    //PtrChangeHandler    m_ptrChangeHandler;
    ArietisEngine *     m_engine;
    Semaphore           m_semaphore;
};

#endif // __ARIETIS_DEBUGGER_H__
