#pragma once

#ifndef __COMMON_HASHTABLE_H__
#define __COMMON_HASHTABLE_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

template <typename T, uint Capacity = 0x10000>
class Hashtable {

    template <typename T> struct Node {
        Node<T> *   Next;
        uint        Index;
        T *         Item;

        Node(uint idx, T *i, Node<T> *n) { 
            Index = idx; Item = i; Next = n; 
        }
    };

public:
    Hashtable()             { ZeroMemory(m_table, Capacity * sizeof(Node<T> *)); }
    virtual ~Hashtable()    { Unload(); }

    bool    Insert(uint index, T *item);
    T *     Lookup(uint index);
    
private:
    void    Unload();
    void    UnloadNode(Node<T> *&node);
private:
    Node<T> *   m_table[Capacity];
};

template <typename T, uint Capacity>
T * LochsEmu::Hashtable<T, Capacity>::Lookup( uint index )
{
    Node<T> *ptr = m_table[index % Capacity];
    while (ptr) {
        if (ptr->Index == index) return ptr->Item;
        ptr = ptr->Next;
    }
    return NULL;
}

template <typename T, uint Capacity>
bool LochsEmu::Hashtable<T, Capacity>::Insert( uint index, T *item )
{
    if (Lookup(index)) return false;
    uint idxTable = index % Capacity;
    m_table[idxTable] = new Node<T>(index, item, m_table[idxTable]);
    return true;
}

template <typename T, uint Capacity>
void LochsEmu::Hashtable<T, Capacity>::UnloadNode( Node<T> *&node )
{
    if (node->Next) {
        UnloadNode(node->Next);
    }
    SAFE_DELETE(node->Item);
    SAFE_DELETE(node);
}

template <typename T, uint Capacity>
void LochsEmu::Hashtable<T, Capacity>::Unload()
{
    for (uint i = 0; i < Capacity; i++) {
        if (m_table[i]) {
            UnloadNode(m_table[i]);
        }
    }
}

END_NAMESPACE_LOCHSEMU()

#endif // __COMMON_HASHTABLE_H__
