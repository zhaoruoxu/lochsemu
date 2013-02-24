#pragma once
 
#ifndef __TAINT_TAINT_H__
#define __TAINT_TAINT_H__
 
#include "Arietis.h"
#include "memory.h"

// Per BYTE Taint structure
class Taint {
public:
    Taint(int nIndices = 32);
    Taint(const Taint &t);
    Taint(Taint &&t);
    Taint &operator=(Taint rhs);
    virtual ~Taint();

    int         GetIndices() const { return m_nIndices; }
    bool        IsTainted(int index) const { 
        Assert(index < m_nIndices); return m_data[index] != 0; 
    }
    void        Set(int index) { 
        Assert(index < m_nIndices); m_data[index] = 1; 
    }
    void        Reset(int index) { 
        Assert(index < m_nIndices); m_data[index] = 0; 
    }
    Taint       operator&(const Taint &rhs) const;
    Taint       operator|(const Taint &rhs) const;
    Taint       operator^(const Taint &rhs) const;
    Taint&      operator&=(const Taint &rhs);
    Taint&      operator|=(const Taint &rhs);
    Taint&      operator^=(const Taint &rhs);

    std::string ToString() const;
private:
    int         m_nIndices;
    pbyte       m_data;
};

class TaintFactory {
public:
    Taint *     Create();
    Taint *     Create(int nIndices);
private:
    FactoryImpl<Taint> m_factory;
};
 
#endif // __TAINT_TAINT_H__