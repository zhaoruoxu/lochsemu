#pragma once

#ifndef __PROPHET_STATIC_DISASSEMBLER_H__
#define __PROPHET_STATIC_DISASSEMBLER_H__

#include "Prophet.h"
#include "instruction.h"
#include "memory.h"
#include "parallel.h"

struct Inst : public Instruction {
    static const int ApiInfoSize = 64;
    u32     Eip;
    u32     Target;
    u32     Entry;
    char    DllName[ApiInfoSize];
    char    FuncName[ApiInfoSize];
    int     Index;

    Inst() : Eip(0), Target(-1), Entry(-1), Index(-1) {
        ZeroMemory(DllName, sizeof(DllName));
        ZeroMemory(FuncName, sizeof(DllName));
    }
};

typedef Inst *  InstPtr;
typedef AllocOnlyPool<Inst>  InstPool;

class InstMem;

class InstSection : public ISyncObject {
public:
    InstSection(InstMem *mem, InstPool &pool, u32 base, u32 size);
    ~InstSection();

    u32             GetBase() const { return m_base; }
    u32             GetSize() const { return m_size; }
    int             GetCount() const { return m_count; }
    InstPtr         GetInst(u32 addr) const 
    {
        //Assert(Contains(addr));
        return m_data[addr - m_base];
    }
    bool            Contains(u32 addr) const 
    {
        AssertInRanage(addr);
        return m_data[addr - m_base] != NULL;
    }
    bool            IsInRange(u32 addr) const
    {
        return m_base <= addr && addr < m_base + m_size; 
    }

    InstPtr         Alloc(u32 addr);
    void            Lock() const;
    void            Unlock() const;

    InstPtr *       Begin() const;
    InstPtr *       Next(InstPtr *curr) const;
    InstPtr *       End() const { return m_data + m_size; }

    void            UpdateIndices() const;
    u32             GetEipFromIndex(int idx) const 
    { 
        Assert(m_indices[idx] != -1);
        return m_indices[idx]; 
    }

private:
    InstSection(const InstSection &);
    InstSection &operator=(const InstSection &);
    void            AssertInRanage(u32 addr) const { Assert(IsInRange(addr)); }
private:
    InstMem *       m_mem;
    u32             m_base;
    u32             m_size;
    InstPtr *       m_data;
    u32 *           m_indices;
    int             m_count;
    InstPool &      m_pool;
};

class InstMem : public MutexSyncObject {
public:
    InstMem();
    ~InstMem();

    InstSection *   GetSection(u32 addr) const { return m_pagetable[PAGE_NUM(addr)]; }
    InstSection *   CreateSection(u32 base, u32 size);

    InstPtr         GetInst(u32 addr) const;
private:
    InstSection *   AddSection(u32 base, u32 size);

private:
    InstSection *   m_pagetable[LX_PAGE_COUNT];
    InstPool        m_pool;
};

class Disassembler {
public:
    typedef std::function<void (InstSection *insts, const Processor *cpu)>    DataUpdateHandler;
    
public:
    Disassembler();
    virtual ~Disassembler();

    void        RegisterDataUpdateHandler(DataUpdateHandler h) {
        m_dataUpdateHandler = h;
    }

    void        OnPreExecute(PreExecuteEvent &event);
    InstPtr     Disassemble(u32 eip);
    void        UpdateInstContext(InstContext *ctx, u32 eip) const;
    InstPtr     GetInst(u32 eip);
private:
    void        RecursiveDisassemble(const Processor *cpu, u32 eip, InstSection *sec, u32 entryEip);
    void        AttachApiInfo(const Processor *cpu, u32 eip, InstSection *sec, InstPtr inst);
private:
    const Processor *   m_currProcessor;
    DataUpdateHandler   m_dataUpdateHandler;
    const Section *     m_lastSec;
    InstMem             m_instMem;
};

#endif // __PROPHET_STATIC_DISASSEMBLER_H__