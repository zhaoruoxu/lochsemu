#pragma once
 
#ifndef __ARIETIS_MEMORY_H__
#define __ARIETIS_MEMORY_H__
 
#include "Arietis.h"

template <typename T>
class AllocOnlyPool {
public:
    AllocOnlyPool(int initialSize);
    ~AllocOnlyPool();

    T *     Alloc();
private:
    void    Expand();
private:
    std::vector<T *>    m_storage;
    T *     m_memory;
    int     m_currSize;
    int     m_currMaxSize;
};



template <typename T>
AllocOnlyPool<T>::AllocOnlyPool( int initialSize )
{
    m_currSize      = 0;
    m_currMaxSize   = initialSize;
    m_memory        = new T[m_currMaxSize];
}

template <typename T>
AllocOnlyPool<T>::~AllocOnlyPool()
{
    SAFE_DELETE_ARRAY(m_memory);
    for (auto &t : m_storage) {
        SAFE_DELETE_ARRAY(t);
    }
    m_storage.clear();
}

template <typename T>
T * AllocOnlyPool<T>::Alloc()
{
    Assert(m_currSize < m_currMaxSize);
    T *res = &m_memory[m_currSize++];
    if (m_currSize == m_currMaxSize)
        Expand();
    return res;
}

template <typename T>
void AllocOnlyPool<T>::Expand()
{
    m_storage.push_back(m_memory);
    m_currMaxSize *= 2;
    m_memory = new T[m_currMaxSize];
    m_currSize = 0;
}


#endif // __ARIETIS_MEMORY_H__