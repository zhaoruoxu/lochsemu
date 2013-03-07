#include "stdafx.h"
#include "taint.h"


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
