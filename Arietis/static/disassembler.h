#pragma once

#ifndef __ARIETIS_STATIC_DISASSEMBLER_H__
#define __ARIETIS_STATIC_DISASSEMBLER_H__

#include "Arietis.h"
#include "instruction.h"

class Disassembler {
public:
    typedef std::shared_ptr<Instruction>    InstPtr;
    struct Inst {
        InstPtr     ptr;
        u32         eip;
        u32         target;
        u32         entry;  // proc entry
        std::string dllName;
        std::string funcName;

        Inst() : ptr(NULL), eip(0), target(-1), entry(-1) {}
        Inst(InstPtr p, u32 e) : ptr(p), eip(e), target(-1), entry(-1) {}
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
    void        UpdateInstContext(InstContext &ctx) const;
    Inst        GetInst(const Processor *cpu, u32 eip);
private:
    void        RecursiveDisassemble(int depth, const Processor *cpu, u32 eip, const Section *sec, u32 entryEip);
    void        AttachApiInfo(const Processor *cpu, u32 eip, const Section *sec, Inst &inst);
private:
    static const int    MaxRecursiveDepth = 3;
    int                 m_currDepth;
    const Processor *   m_currProcessor;
    DataUpdateHandler   m_dataUpdateHandler;
    const Section *     m_lastSec;
    //InstDisasmMap       m_instMap;
    std::unordered_map<const Section *, InstDisasmMap> m_secMap;
};

#endif // __ARIETIS_STATIC_DISASSEMBLER_H__
