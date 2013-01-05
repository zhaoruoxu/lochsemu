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
        Inst(InstPtr ptr, u32 e):instPtr(ptr), eip(e) {}
    };
    typedef std::vector<Inst>       InstVector;
    
    typedef std::function<void (const InstVector &vec)>     InstDisasmHandler;
    typedef std::function<void (u32 addr)>                  PtrChangeHandler;
    typedef std::map<u32, InstPtr>  InstDisasmMap;
public:
    Disassembler();
    virtual ~Disassembler();

    void        RegisterInstDisasmHandler(InstDisasmHandler h) { m_instDisasmHandler = h; }
    void        RegisterPtrChangeHandler(PtrChangeHandler h) { m_ptrChangeHandler = h; }

    void        OnPreExecute(const Processor *cpu, const Instruction *inst);
private:
    void        RecursiveDisassemble(const Processor *cpu, u32 eip, const Section *sec);
private:
    InstDisasmHandler   m_instDisasmHandler;
    PtrChangeHandler    m_ptrChangeHandler;
    InstDisasmMap       m_instMap;
    InstVector          m_instVecTemp;
};

#endif // __ARIETIS_STATIC_DISASSEMBLER_H__
