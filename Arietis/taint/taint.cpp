#include "stdafx.h"
#include "taint.h"


Taint::Taint( int nPieces )
    : m_nPieces(nPieces)
{
    m_data          = new byte[nPieces];
    ZeroMemory(m_data, sizeof(byte) * m_nPieces);
}

Taint::Taint( const Taint &t )
{
    m_nPieces       = t.m_nPieces;
    m_data          = new byte[m_nPieces];
    memcpy(m_data, t.m_data, m_nPieces);
}

Taint::Taint( Taint &&t )
{
    m_data          = t.m_data;
    m_nPieces       = t.m_nPieces;

    t.m_data        = NULL;
    t.m_nPieces     = 0;
}

Taint & Taint::operator=( Taint rhs )
{
    std::swap(m_nPieces, rhs.m_nPieces);
    std::swap(m_data, rhs.m_data);
    return *this;
}

Taint::~Taint()
{
    SAFE_DELETE_ARRAY(m_data);
}
