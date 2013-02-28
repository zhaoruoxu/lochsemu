#pragma once
 
#ifndef __ARIETIS_TAINT_COMPTAINT_H__
#define __ARIETIS_TAINT_COMPTAINT_H__
 
#include "Arietis.h"
#include "taint.h"
#include "instcontext.h"

struct ProcessorTaint {
    Taint4      GPRegs[8];
    Taint       Flags[InstContext::FLAG_COUNT];
    Taint       Eip;
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

    //Taint       Get(u32 addr);
    //Taint       Get(u32 addr, u32 len);     // 'OR' these Taint value
    //void        Set(const Taint &t, u32 addr, u32 len = 1);

    Taint       Get1(u32 addr);
    Taint2      Get2(u32 addr);
    Taint4      Get4(u32 addr);
    void        Set1(u32 addr, const Taint  &t);
    void        Set2(u32 addr, const Taint2 &t);
    void        Set4(u32 addr, const Taint4 &t);

private:
    PageTaint *  GetPage(u32 addr);

private:
    //std::unordered_map<u32, Taint> m_status;
    PageTaint *  m_pagetable[LX_PAGE_COUNT];
    //TaintFactory &  m_factory;
};


#endif // __ARIETIS_TAINT_COMPTAINT_H__