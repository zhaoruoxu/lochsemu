#pragma once

#ifndef __ARIETIS_DEBUGGER_H__
#define __ARIETIS_DEBUGGER_H__

#include "Arietis.h"
#include "parallel.h"

class Debugger {
public:
    Debugger(ArietisEngine *engine);
    ~Debugger();

    void        Initialize();
    void        OnPreExecute(Processor *cpu, const Instruction *inst);
    void        OnPostExecute(Processor *cpu, const Instruction *inst);
    void        OnStepInto();

private:
    ArietisEngine * m_engine;
    Semaphore   m_semaphore;
};

#endif // __ARIETIS_DEBUGGER_H__
