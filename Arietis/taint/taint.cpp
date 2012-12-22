#include "stdafx.h"
#include "taint.h"


Taint::Taint( int nIndices )
    : m_nIndices(nIndices)
{
    m_data          = new byte[nIndices];
    ZeroMemory(m_data, sizeof(byte) * m_nIndices);
}

Taint::Taint( const Taint &t )
{
    m_nIndices      = t.m_nIndices;
    m_data          = new byte[m_nIndices];
    memcpy(m_data, t.m_data, m_nIndices);
}

Taint::Taint( Taint &&t )
{
    m_data          = t.m_data;
    m_nIndices      = t.m_nIndices;

    t.m_data        = NULL;
    t.m_nIndices    = 0;
}

Taint & Taint::operator=( Taint rhs )
{
    std::swap(m_nIndices, rhs.m_nIndices);
    std::swap(m_data, rhs.m_data);
    return *this;
}

Taint::~Taint()
{
    SAFE_DELETE_ARRAY(m_data);
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
    Assert(m_nIndices == rhs.m_nIndices);
    for (int i = 0; i < m_nIndices; i++)
        m_data[i] &= rhs.m_data[i];
    return *this;
}

Taint& Taint::operator|=( const Taint &rhs )
{
    Assert(m_nIndices == rhs.m_nIndices);
    for (int i = 0; i < m_nIndices; i++)
        m_data[i] |= rhs.m_data[i];
    return *this;
}

Taint& Taint::operator^=( const Taint &rhs )
{
    Assert(m_nIndices == rhs.m_nIndices);
    for (int i = 0; i < m_nIndices; i++)
        m_data[i] ^= rhs.m_data[i];
    return *this;
}

std::string Taint::ToString() const
{
    std::string r;
    for (int i = 0; i < m_nIndices; i++)
        r += ('0' + m_data[i]);
    return r;
}
