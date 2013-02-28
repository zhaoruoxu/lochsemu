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
struct TB {
    static const int    Count = N;
    Taint       T[N];

    TB<N>   operator&(const TB<N> &rhs) const
    {
        TB<N>   res = *this;
        res &= rhs;
        return res;
    }

    TB<N>   operator|(const TB<N> &rhs) const
    {
        TB<N>   res = *this;
        res |= rhs;
        return res;
    }

    TB<N>   operator^(const TB<N> &rhs) const
    {
        TB<N>   res = *this;
        res ^= rhs;
        return res;
    }

    TB<N>&  operator&=(const TB<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    &= rhs.T[i];
        return *this;
    }

    TB<N>&  operator|=(const TB<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    &= rhs.T[i];
        return *this;
    }

    TB<N>&  operator^=(const TB<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    &= rhs.T[i];
        return *this;
    }
};

typedef Taint   Taint1;
typedef TB<4>   Taint4;
typedef TB<2>   Taint2;

//  Retrieve from Taint32
Taint2     ToTaint2(const Taint4 &t, int offset = 0);

//  Set part of Taint32 from Taint16
void        FromTaint2(Taint4 &dest, const Taint2 &src, int offset = 0);

Taint2      PackTaint(const Taint &t0, const Taint &t1);
Taint4      PackTaint(const Taint &t0, const Taint &t1, const Taint &t2, const Taint &t3);
Taint       Shrink(const Taint4 &t);
Taint       Shrink(const Taint2 &t);

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