#include "stdafx.h"
#include "heap.h"
#include "memory.h"

BEGIN_NAMESPACE_LOCHSEMU()


Heap::Heap( u32 base, u32 reserve, u32 commit, uint nModule )
: Section(SectionDesc("heap", nModule), base, reserve)
{
    Assert(PAGE_LOW(base) == 0);
    Assert(PAGE_LOW(reserve) == 0);
    Assert(PAGE_LOW(commit) == 0);

    m_map = new uint[m_pages];
    ZeroMemory(m_map, sizeof(uint) * m_pages);
}

Heap::~Heap()
{
    SAFE_DELETE_ARRAY(m_map);
}

u32 Heap::HeapAlloc( u32 size, uint flags, Processor *cpu )
{
    u32 actualSize = RoundUp(size);
    if (actualSize == 0) return 0;
    u32 pageSize = PAGE_NUM(actualSize);
    uint startPage = FindSpace(pageSize);
    if (startPage > m_pages) {
        // TODO: cpu exception
        return 0;
    }
    u32 startAddr = PAGE_ADDR(startPage) + m_base;
    V( Commit(startAddr, actualSize, PAGE_READWRITE) ); // TODO: may change the page flag

    for (uint i = startPage; i < startPage + pageSize; i++) {
        Assert(m_map[i] == 0);
        m_map[i] = pageSize;
    }
    m_memBlockSize[startAddr] = size;
    return startAddr;
}

bool Heap::HeapFree( u32 addr, uint flags, Processor *cpu )
{
    // addr must be the start of a page
    if (PAGE_LOW(addr) != 0) return false;

    if (!Contains(addr)) return false;

    uint startPage = PAGE_NUM(addr - m_base);
    uint size = m_map[startPage];
    if (size == 0) return false;

    for (uint i = startPage; i < size+startPage; i++) {
        Assert(m_map[i] == size);
        m_map[i] = 0;
    }
    if (LX_FAILED(Decommit(addr, PAGE_ADDR(size))))
        return false;
    return true;
}

u32 Heap::HeapRealloc( u32 addr, u32 size, uint flags, Processor *cpu )
{
    if (PAGE_LOW(addr) != 0) return 0;
    const u32 actualSize = RoundUp(size);

    if (!Contains(addr)) return 0;
    if (actualSize == 0) return 0;
    
    u32 newAddr = HeapAlloc(actualSize, flags, cpu);
    if (newAddr == 0) return 0;

    const u32 origSize = PAGE_ADDR(m_map[PAGE_NUM(addr-m_base)]);

    memcpy(m_dataPtr + (newAddr - m_base), m_dataPtr + (addr - m_base), min(origSize, actualSize));

    if (!HeapFree(addr, 0, cpu)) return 0;
    return newAddr;
}

u32 Heap::HeapSize( u32 addr, uint flags, Processor *cpu )
{
    Assert(m_memBlockSize.find(addr) != m_memBlockSize.end());
    return m_memBlockSize[addr];
//     if (PAGE_LOW(addr) != 0) return 0;
//     uint page = PAGE_NUM(addr - m_base);
//     return m_map[page];
}

bool Heap::HeapValidate( u32 addr, uint flags, Processor *cpu )
{
    LxWarning("Heap::HeapValidate()\n");
    return true;
}

uint Heap::FindSpace( uint nPages )
{
    uint ptr = 0;
    uint contBlocks = 0;
    while (ptr < m_pages) {
        if (m_map[ptr] == 0) {
            contBlocks++;
            ptr++;
            if (contBlocks == nPages) return ptr - nPages;
        } else {
            contBlocks = 0;
            ptr += m_map[ptr];
        }
    }
    return m_pages + 1;
}

END_NAMESPACE_LOCHSEMU()