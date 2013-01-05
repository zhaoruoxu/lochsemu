#pragma once

#ifndef __ARIETIS_STATIC_DISASSEMBLER_H__
#define __ARIETIS_STATIC_DISASSEMBLER_H__

#include "Arietis.h"

class Disassembler {
public:
    typedef std::shared_ptr<Instruction>    InstPtr;
    struct Inst {
        InstPtr     instPtr;
        u32         eip;
        Inst() : instPtr(NULL), eip(0) {}
        Inst(InstPtr ptr, u32 e):instPtr(ptr), eip(e) {}
    };
    typedef std::vector<Inst>       InstVector;
    typedef std::map<u32, Inst>     InstDisasmMap;
    
    typedef std::function<void (const InstVector &vec)>     InstDisasmHandler;
    typedef std::function<void (u32 addr)>                  PtrChangeHandler;
    typedef std::function<void (const InstDisasmMap *insts)>    DataUpdateHandler;
    
public:
    Disassembler();
    virtual ~Disassembler();

    //void        RegisterInstDisasmHandler(InstDisasmHandler h) { m_instDisasmHandler = h; }
    void        RegisterPtrChangeHandler(PtrChangeHandler h) { 
        m_ptrChangeHandler = h; 
    }
    void        RegisterDataUpdateHandler(DataUpdateHandler h) {
        m_dataUpdateHandler = h;
    }

    void        OnPreExecute(const Processor *cpu, const Instruction *inst);
private:
    void        RecursiveDisassemble(const Processor *cpu, u32 eip, const Section *sec);
private:
    PtrChangeHandler    m_ptrChangeHandler;
    DataUpdateHandler   m_dataUpdateHandler;
    InstDisasmMap       m_instMap;
};

#endif // __ARIETIS_STATIC_DISASSEMBLER_H__
