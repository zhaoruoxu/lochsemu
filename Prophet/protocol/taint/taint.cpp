#include "stdafx.h"
#include "taint.h"
#include "utilities.h"

Taint::Taint( )
{
    ZeroMemory(m_data, sizeof(m_data));
}

Taint::Taint( const Taint &t )
{
    memcpy(m_data, t.m_data, sizeof(m_data));
//     for (int i = 0; i < Count; i++)
//         m_data[i] = t.m_data[i];
}

Taint & Taint::operator=( Taint rhs )
{
    memcpy(m_data, rhs.m_data, sizeof(m_data));
    return *this;
}

Taint::~Taint()
{
    //SAFE_DELETE_ARRAY(m_data);
}

Taint Taint::operator&( const Taint &rhs ) const
{
    Taint res = *this;
    res &= rhs;
    return res;
}

Taint Taint::operator|( const Taint &rhs ) const
{
    Taint res = *this;
    res |= rhs;
    return res;
}

Taint Taint::operator^( const Taint &rhs ) const
{
    Taint res = *this;
    res ^= rhs;
    return res;
}

Taint Taint::operator~() const
{
    Taint t = *this;
    for (int i = 0; i < Count; i++)
        t.m_data[i] = ~t.m_data[i];
    return t;
}

Taint& Taint::operator&=( const Taint &rhs )
{
    for (int i = 0; i < Count; i++)
        m_data[i] &= rhs.m_data[i];
    return *this;
}

Taint& Taint::operator|=( const Taint &rhs )
{
    for (int i = 0; i < Count; i++)
        m_data[i] |= rhs.m_data[i];
    return *this;
}

Taint& Taint::operator^=( const Taint &rhs )
{
    for (int i = 0; i < Count; i++)
        m_data[i] ^= rhs.m_data[i];
    return *this;
}

std::string Taint::ToString() const
{
    std::string r;
    for (int i = 0; i < Width; i++)
        r += (char) ('0' + (IsTainted(i) ? 1 : 0));
    return r;
}

Taint Taint::FromBinString( const std::string &s )
{
    Taint r;
    if (s.size() > Width) {
        LxFatal("Size of string exceeded width of Taint\n");
    }
    for (uint i = 0; i < s.size(); i++) {
        int val = s[i] - '0';
        if (val != 0 && val != 1)
            LxFatal("Invalid character %c in string %s\n", s[i], s);
        if (val != 0) r.Set(i);
    }
    return r;
}

void Taint::Dump( File &f ) const
{
    bool prev = IsTainted(0);
    int start = 0;
    for (int i = 1; i <= Width; i++) {
        if (i != Width && IsTainted(i)) {
            if (!prev) { start = i; }
            prev = true;
        } else if (prev) {
            if (start == i-1)
                fprintf(f.Ptr(), "%d ", start);
            else
                fprintf(f.Ptr(), "%d-%d ", start, i-1);
            prev = false;
        }
    }
    if (!IsAnyTainted())
        fprintf(f.Ptr(), "None");
    fprintf(f.Ptr(), "\n");
}

std::vector<TaintRegion> Taint::GenerateRegions() const
{
    std::vector<TaintRegion> r;
    bool prev = IsTainted(0);
    int start = 0;
    for (int i = 1; i <= Width; i++) {
        if (i != Width && IsTainted(i)) {
            if (!prev) { start = i; }
            prev = true;
        } else if (prev) {
            r.emplace_back(start, i - start);
            prev = false;
        }
    }
    return r;
}


void GetTaintRange( const Taint &t, int *firstIndex, int *lastIndex )
{
    if (firstIndex) {
        *firstIndex = -1;
        for (int i = 0; i < t.GetWidth(); i++) {
            if (t.IsTainted(i)) {
                *firstIndex = i;
                break;
            }
        }
    }

    if (lastIndex) {
        *lastIndex = -1;
        for (int i = t.GetWidth() - 1; i >= 0; i--) {
            if (t.IsTainted(i)) {
                *lastIndex = i;
                break;
            }
        }
    }
}
