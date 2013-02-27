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
struct TaintBlock {
    static const int    Count = N;
    Taint       T[N];

    TaintBlock<N>   operator&(const TaintBlock<N> &rhs) const
    {
        TaintBlock<N>   res = *this;
        res &= rhs;
        return res;
    }

    TaintBlock<N>   operator|(const TaintBlock<N> &rhs) const
    {
        TaintBlock<N>   res = *this;
        res |= rhs;
        return res;
    }

    TaintBlock<N>   operator^(const TaintBlock<N> &rhs) const
    {
        TaintBlock<N>   res = *this;
        res ^= rhs;
        return res;
    }

    TaintBlock<N>&  operator&=(const TaintBlock<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    &= rhs.T[i];
        return *this;
    }

    TaintBlock<N>&  operator|=(const TaintBlock<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    &= rhs.T[i];
        return *this;
    }

    TaintBlock<N>&  operator^=(const TaintBlock<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    &= rhs.T[i];
        return *this;
    }
};

typedef Taint           Taint8;
typedef TaintBlock<4>   Taint32;
typedef TaintBlock<2>   Taint16;

Taint16     ToTaint16(const Taint32 &t, int offset = 0);

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