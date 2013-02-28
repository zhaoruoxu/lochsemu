#pragma once
 
#ifndef __TAINT_TAINT_H__
#define __TAINT_TAINT_H__
 
#include "Arietis.h"
#include "memory.h"

// Per BYTE Taint structure
class Taint {
public:
    
    Taint();
    Taint(const Taint &t);
    //Taint(Taint &&t);
    Taint &operator=(Taint rhs);
    virtual ~Taint();

    int         GetWidth() const { return Width; }

    //int         GetIndices() const { return m_nIndices; }
    bool        IsTainted(int index) const { 
        Assert(index < Width); return m_data[index] != 0; 
    }
    void        Set(int index) { 
        Assert(index < Width); m_data[index] = 1; 
    }

    void        SetAll() {
        memset(m_data, 1, sizeof(m_data));
    }

    void        ResetAll() {
        memset(m_data, 0, sizeof(m_data));
    }

    void        Reset(int index) { 
        Assert(index < Width); m_data[index] = 0; 
    }
    Taint       operator&(const Taint &rhs) const;
    Taint       operator|(const Taint &rhs) const;
    Taint       operator^(const Taint &rhs) const;
    Taint&      operator&=(const Taint &rhs);
    Taint&      operator|=(const Taint &rhs);
    Taint&      operator^=(const Taint &rhs);

    std::string ToString() const;
    static Taint    FromBinString(const std::string &s);
private:
    //int         m_nIndices;
    static const int    Width = 1;
    byte        m_data[Width];
};

template <int N>
struct Tb {
    static const int    Count = N;
    Taint       T[N];

    Tb<N>   operator&(const Tb<N> &rhs) const
    {
        Tb<N>   res = *this;
        res &= rhs;
        return res;
    }

    Tb<N>   operator|(const Tb<N> &rhs) const
    {
        Tb<N>   res = *this;
        res |= rhs;
        return res;
    }

    Tb<N>   operator^(const Tb<N> &rhs) const
    {
        Tb<N>   res = *this;
        res ^= rhs;
        return res;
    }

    Tb<N>&  operator&=(const Tb<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    &= rhs.T[i];
        return *this;
    }

    Tb<N>&  operator|=(const Tb<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    |= rhs.T[i];
        return *this;
    }

    Tb<N>&  operator^=(const Tb<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    ^= rhs.T[i];
        return *this;
    }

    void        ResetAll() 
    {
        for (int i = 0; i < N; i++)
            T[i].ResetAll();
    }

    void        SetAll()
    {
        for (int i = 0; i < N; i++)
            T[i].SetAll();
    }
};

typedef Tb<1>   Taint1;
typedef Tb<4>   Taint4;
typedef Tb<2>   Taint2;

template <int Nsrc, int Ndest>
Tb<Ndest>   FromTaint(const Tb<Nsrc> &src, int offset = 0)
{
    Assert(Ndest + offset <= Nsrc);

    Tb<Ndest> res;
    for (int i = 0; i < Ndest; i++)
        res.T[i] = src.T[i+offset];
    return res;
}

template <int Nsrc, int Ndest>
void        ToTaint(Tb<Ndest> &dest, const Tb<Nsrc> &src, int offset = 0)
{
    Assert(Nsrc + offset <= Ndest);
    for (int i = 0; i < Nsrc; i++)
        dest.T[i+offset] = src.T[i];
}

template <int N>
Taint1      Shrink(const Tb<N> &t)
{
    Taint1 res;
    res.T[0] = t.T[0];
    for (int i = 1; i < N; i++)
        res.T[0] |= t.T[i];
    return res;
}


//  Retrieve from Taint32
// Taint2     ToTaint2(const Taint4 &t, int offset = 0);
// 
//  Set part of Taint32 from Taint16
// void        FromTaint2(Taint4 &dest, const Taint2 &src, int offset = 0);
// 
// Taint2      PackTaint(const Taint &t0, const Taint &t1);
// Taint4      PackTaint(const Taint &t0, const Taint &t1, const Taint &t2, const Taint &t3);
// Taint       Shrink(const Taint4 &t);
// Taint       Shrink(const Taint2 &t);

// struct Taint4 {
//     static const int    Count = 4;
//     Taint       T[Count];
// 
//     Taint4    operator&(const Taint4 &rhs) const;
//     Taint4    operator|(const Taint4 &rhs) const;
//     Taint4    operator^(const Taint4 &rhs) const;
//     Taint4&   operator&=(const Taint4 &rhs);
//     Taint4&   operator|=(const Taint4 &rhs);
//     Taint4&   operator^=(const Taint4 &rhs);
// 
// 
// };

// class TaintFactory {
// public:
//     Taint *     Create();
//     //Taint *     Create(int nIndices);
// private:
//     FactoryImpl<Taint> m_factory;
// };
 
// typedef Taint *     TaintPtr;
// 不应该使用AllocOnlyPool, Taint所使用内存会动态调整

#endif // __TAINT_TAINT_H__