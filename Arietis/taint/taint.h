#pragma once
 
#ifndef __TAINT_TAINT_H__
#define __TAINT_TAINT_H__
 
#include "Arietis.h"


// Per BYTE Taint structure
class Taint {
public:
    Taint(int nPieces);
    Taint(const Taint &t);
    Taint(Taint &&t);
    Taint &operator=(Taint rhs);
    virtual ~Taint();

    int     GetPieces() const { return m_nPieces; }
private:
    int         m_nPieces;
    pbyte       m_data;
};
 
#endif // __TAINT_TAINT_H__