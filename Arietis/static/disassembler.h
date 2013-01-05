#pragma once

#ifndef __ARIETIS_STATIC_DISASSEMBLER_H__
#define __ARIETIS_STATIC_DISASSEMBLER_H__

#include "Arietis.h"

class Disassembler {
public:
    typedef std::shared_ptr<Instruction>    InstPtr;
    struct Inst {
        InstPtr     ptr;
        u32         eip;
        u32         target;
        std::string dllName;
        std::string funcName;

        Inst() : ptr(NULL), eip(0) {}
        Inst(InstPtr ptr, u32 e):ptr(ptr), eip(e) {}
    };
    typedef std::vector<Inst>       InstVector;
    typedef std::map<u32, Inst>     InstDisasmMap;
    
    typedef std::function<void (const InstVector &vec)>     InstDisasmHandler;
    
    typedef std::function<void (const InstDisasmMap *insts)>    DataUpdateHandler;
    
public:
    Disassembler();
    virtual ~Disassembler();

    //void        RegisterInstDisasmHandler(InstDisasmHandler h) { m_instDisasmHandler = h; }

    void        RegisterDataUpdateHandler(DataUpdateHandler h) {
        m_dataUpdateHandler = h;
    }

    void        OnPreExecute(const Processor *cpu, const Instruction *inst);
private:
    void        RecursiveDisassemble(const Processor *cpu, u32 eip, const Section *sec);
    void        AttachApiInfo(const Processor *cpu, u32 eip, const Section *sec, Inst &inst);
private:
    DataUpdateHandler   m_dataUpdateHandler;
    const Section *     m_lastSec;
    //InstDisasmMap       m_instMap;
    std::unordered_map<const Section *, InstDisasmMap> m_secMap;
};

#endif // __ARIETIS_STATIC_DISASSEMBLER_H__
