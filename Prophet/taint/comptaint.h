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

    void        Reset();
    ProcessorTaint *    Clone() const;
    void        CopyFrom(const ProcessorTaint *t);
    void        Dump(FILE *fp) const;
};

class MemoryTaint {

    struct PageTaint { 
        Taint   Get(u32 offset);
        void    Set(u32 offset, const Taint &t);
        void    Reset();

        PageTaint * Clone() const;
        void        CopyFrom(const PageTaint *t);
        void        Dump(FILE *fp, u32 base) const;

        PageTaint();
        ~PageTaint();

    private:
        Taint *     m_data[LX_PAGE_SIZE];
        //FilePool<Taint> &   m_pool;
    };


public:
    MemoryTaint();
    ~MemoryTaint();

    //void        Init(const char *pool);

    template <int N>
    Tb<N>       Get(u32 addr);

    template <int N>
    void        Set(u32 addr, const Tb<N> &t);

    void        Reset();

    MemoryTaint *   Clone() const;
    void            CopyFrom(const MemoryTaint *t);

    void            Dump(FILE *fp) const;
private:
    PageTaint *  GetPage(u32 addr);

private:
    PageTaint * m_pagetable[LX_PAGE_COUNT];
    //FilePool<Taint>    m_pool;
};

template <int N>
Tb<N> MemoryTaint::Get( u32 addr )
{
    Tb<N> res;
    for (int i = 0; i < N; i++) {
        res[i]  = GetPage(addr+i)->Get(PAGE_LOW(addr+i));
    }
    return res;
}

template <int N>
void MemoryTaint::Set( u32 addr, const Tb<N> &t )
{
    for (int i = 0; i < N; i++) {
        GetPage(addr+i)->Set(PAGE_LOW(addr+i), t[i]);
    }
}

#endif // __PROPHET_TAINT_COMPTAINT_H__