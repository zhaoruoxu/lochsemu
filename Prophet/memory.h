#pragma once
 
#ifndef __PROPHET_MEMORY_H__
#define __PROPHET_MEMORY_H__
 
#include "prophet.h"

template <typename T>
class AllocOnlyPool {
public:
    AllocOnlyPool(int initialSize)
    {
        m_currSize      = 0;
        m_currMaxSize   = initialSize;
        m_memory        = new T[m_currMaxSize];
    }

    ~AllocOnlyPool()
    {
        SAFE_DELETE_ARRAY(m_memory);
        for (auto &t : m_storage) {
            SAFE_DELETE_ARRAY(t);
        }
        m_storage.clear();
    }

    T *     Alloc()
    {
        Assert(m_currSize < m_currMaxSize);
        T *res = &m_memory[m_currSize++];
        if (m_currSize == m_currMaxSize)
            Expand();
        return res;
    }

private:
    void    Expand()
    {
        m_storage.push_back(m_memory);
        m_currMaxSize *= 2;
        m_memory = new T[m_currMaxSize];
        m_currSize = 0;
    }
private:
    std::vector<T *>    m_storage;
    T *     m_memory;
    int     m_currSize;
    int     m_currMaxSize;
};


template <typename T>
class FactoryImpl {
public:
    FactoryImpl() { m_storage.reserve(16); }
    ~FactoryImpl() 
    {
        for (auto &t : m_storage) {
            SAFE_DELETE(t);
        }
        m_storage.clear();
    }

    T* Keep(T *p)
    {
        m_storage.push_back(p);
        return p;
    }

    int Size() const { return m_storage.size(); }

private:
    std::vector<T *>    m_storage;

    FactoryImpl(const FactoryImpl &);
    FactoryImpl& operator=(const FactoryImpl &);
};


#endif // __PROPHET_MEMORY_H__