#pragma once
 
#ifndef __PROPHET_TAINT_COMPTAINT_H__
#define __PROPHET_TAINT_COMPTAINT_H__
 
#include "prophet.h"
#include "taint.h"
#include "instcontext.h"

struct ProcessorTaint {
    Taint4      GPRegs[8];
    Taint1      Flags[InstContext::FLAG_COUNT];
    Taint1      Eip;
    Taint8      MM[8];
    Taint16     XMM[8];
};

class MemoryTaint {

    class PageTaint { 
    public:
        PageTaint();
        ~PageTaint();

        Taint   Get(u32 offset);
        void    Set(u32 offset, const Taint &t);
    private:
        Taint           m_data[LX_PAGE_SIZE];
    };


public:
    MemoryTaint();
    ~MemoryTaint();

    template <int N>
    Tb<N>       Get(u32 addr);

    template <int N>
    void        Set(u32 addr, const Tb<N> &t);

private:
    PageTaint *  GetPage(u32 addr);

private:
    PageTaint *  m_pagetable[LX_PAGE_COUNT];
};

template <int N>
Tb<N> MemoryTaint::Get( u32 addr )
{
    Tb<N> res;
    for (int i = 0; i < N; i++) {
        res.T[i]    = GetPage(addr+i)->Get(PAGE_LOW(addr+i));
    }
    return res;
}

template <int N>
void MemoryTaint::Set( u32 addr, const Tb<N> &t )
{
    for (int i = 0; i < N; i++) {
        GetPage(addr+i)->Set(PAGE_LOW(addr+i), t.T[i]);
    }
}

#endif // __PROPHET_TAINT_COMPTAINT_H__