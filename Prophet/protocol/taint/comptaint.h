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
    void        Dump(File &f) const;
};

class MemoryTaint {

    struct PageTaint { 
        Taint   Get(u32 offset);
        void    Set(u32 offset, const Taint &t);
        void    Reset();

        PageTaint * Clone() const;
        void        CopyFrom(const PageTaint *t);
        void        Dump(File &f, u32 base) const;

        PageTaint();
        ~PageTaint();

    private:
        Taint *     m_data[LX_PAGE_SIZE];
    };


public:
    MemoryTaint();
    ~MemoryTaint();

    template <int N>
    Tb<N>       Get(u32 addr);

    template <int N>
    void        Set(u32 addr, const Tb<N> &t);

    Taint       GetByte(u32 addr);
    void        SetByte(u32 addr, const Taint &t);

    void        Reset();

    MemoryTaint *   Clone() const;
    void            CopyFrom(const MemoryTaint *t);

    void            Dump(File &f) const;
private:
    PageTaint *  GetPage(u32 addr);

private:
    static const u32 Pages = LX_PAGE_COUNT/2;   // No address above 0x7fffffff
    PageTaint ** m_pagetable;
};

template <int N>
Tb<N> MemoryTaint::Get( u32 addr )
{
    Assert((addr & 0x80000000) == 0);
    Tb<N> res;
    for (int i = 0; i < N; i++) {
        res[i]  = GetByte(addr+i);
    }
    return res;
}

template <int N>
void MemoryTaint::Set( u32 addr, const Tb<N> &t )
{
    Assert((addr & 0x80000000) == 0);
    for (int i = 0; i < N; i++) {
        SetByte(addr+i, t[i]);
    }
}

#endif // __PROPHET_TAINT_COMPTAINT_H__