#pragma once
 
#ifndef __TAINT_TAINT_H__
#define __TAINT_TAINT_H__
 
#include "prophet.h"
#include "memory.h"

// Per BYTE Taint structure
class Taint {
public:
    
    Taint();
    Taint(const Taint &t);
    Taint &operator=(Taint rhs);
    virtual ~Taint();

    static int  GetWidth() { return Width; }

    bool        IsTainted(int index) const { 
        Assert(index < Width);
        int i = index / 32;
        int s = index % 32;
        return ((m_data[i] >> s) & 1) != 0;
    }

    bool        IsAllTainted() const {
        for (int i = 0; i < Count; i++)
            if (m_data[i] != 0xffffffff) return false;
        return true;
    }

    bool        IsAllUntainted() const {
        for (int i = 0; i < Count; i++)
            if (m_data[i] != 0) return false;
        return true;
    }

    bool        IsAnyTainted() const {
        for (int i = 0; i < Count; i++)
            if (m_data[i] != 0) return true;
        return false;
    }

    bool        IsRangeAllTainted(int first, int last) const {
        for (int i = first; i <= last; i++)
            if (!IsTainted(i)) return false;
        return true;
    }

    bool        IsRangeAllUntainted(int first, int last) const {
        for (int i = first; i <= last; i++)
            if (IsTainted(i)) return false;
        return true;
    }

    void        Set(int index) { 
        Assert(index < Width); 
        int i = index / 32;
        int s = index % 32;
        m_data[i] |= (1 << s);
    }

    void        Reset(int index) { 
        Assert(index < Width); 
        int i = index / 32;
        int s = index % 32;
        m_data[i] &= ~(1 << s);
    }

    void        SetAll() {
        memset(m_data, 0xff, sizeof(m_data));
    }

    void        ResetAll() {
        memset(m_data, 0, sizeof(m_data));
    }

    Taint       operator&(const Taint &rhs) const;
    Taint       operator|(const Taint &rhs) const;
    Taint       operator^(const Taint &rhs) const;
    Taint&      operator&=(const Taint &rhs);
    Taint&      operator|=(const Taint &rhs);
    Taint&      operator^=(const Taint &rhs);

    std::string ToString() const;
    void        Dump(File &f) const;
    static Taint    FromBinString(const std::string &s);

public:
    static const int    Count = 16;
    static const int    Width = 32 * Count;
private:
    u32         m_data[Count];
};

void    GetTaintRange(const Taint &t, int *firstIndex, int *lastIndex);

template <int N>
struct Tb {
    static const int    Count = N;
    Taint       T[N];

    Taint & operator[](int n)
    {
        Assert(n >= 0 && n < N);
        return T[n];
    }

    const Taint &   operator[](int n) const
    {
        Assert(n >= 0 && n < N);
        return T[n];
    }

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
            T[i]    &= rhs[i];
        return *this;
    }

    Tb<N>&  operator|=(const Tb<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    |= rhs[i];
        return *this;
    }

    Tb<N>&  operator^=(const Tb<N> &rhs)
    {
        for (int i = 0; i < N; i++)
            T[i]    ^= rhs[i];
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

    bool        IsAnyTainted() const
    {
        for (int i = 0; i < N; i++)
            if (T[i].IsAnyTainted()) return true;
        return false;
    }
};

typedef Tb<1>   Taint1;
typedef Tb<4>   Taint4;
typedef Tb<2>   Taint2;
typedef Tb<8>   Taint8;
typedef Tb<16>  Taint16;

template <int Nsrc, int Ndest>
Tb<Ndest>   FromTaint(const Tb<Nsrc> &src, int offset = 0)
{
    Assert(Ndest + offset <= Nsrc);

    Tb<Ndest> res;
    for (int i = 0; i < Ndest; i++)
        res[i] = src[i+offset];
    return res;
}

template <int Nsrc, int Ndest>
void        ToTaint(Tb<Ndest> &dest, const Tb<Nsrc> &src, int offset = 0)
{
    Assert(Nsrc + offset <= Ndest);
    for (int i = 0; i < Nsrc; i++)
        dest[i+offset] = src[i];
}

template <int N>
Tb<N>       Extend(const Taint1 &t)
{
    Tb<N> res;
    for (int i = 0; i < N; i++)
        res[i] = t[0];
    return res;
}

template <int N>
Taint1      Shrink(const Tb<N> &t)
{
    Taint1 res;
    res[0] = t[0];
    for (int i = 1; i < N; i++)
        res[0] |= t[i];
    return res;
}


#endif // __TAINT_TAINT_H__