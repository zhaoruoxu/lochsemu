#pragma once

#ifndef __CORE_HEAP_H__
#define __CORE_HEAP_H__

#include "lochsemu.h"
#include "section.h"

BEGIN_NAMESPACE_LOCHSEMU()

class LX_API Heap : public Section {
public:
    Heap(u32 base, u32 reserve, u32 commit, uint nModule);
    virtual ~Heap();

    u32     HeapAlloc(u32 size, uint flags, Processor *cpu);
    bool    HeapFree(u32 addr, uint flags, Processor *cpu);
    u32     HeapRealloc(u32 addr, u32 size, uint flags, Processor *cpu);
    u32     HeapSize(u32 addr, uint flags, Processor *cpu);
    bool    HeapValidate(u32 addr, uint flags, Processor *cpu);
private:
    /*
     * Find next continuous pages available;
     * if found, retval = page starting number;
     * if not found, retval = n > m_pages
     */
    uint FindSpace(uint nPages);
private:
    uint *m_map;
    std::map<u32, u32>  m_memBlockSize; // Preserve the size of each memory block
};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_HEAP_H__
