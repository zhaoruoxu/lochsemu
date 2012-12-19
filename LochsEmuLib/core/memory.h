#pragma once

#ifndef __CORE_MEMORY_H__
#define __CORE_MEMORY_H__

#include "lochsemu.h"
#include "section.h"
#include "debug.h"

BEGIN_NAMESPACE_LOCHSEMU()

class LX_API Memory {
    // Simulate x86 RAM
    // A naive implementation; may optimize this
    friend class Processor;
public:
    Memory();
    virtual ~Memory();

public:
    /*
     * Reserve and commit a memory section; may contain several pages
     */
    LxResult        Alloc           (   const SectionDesc &desc, u32 address, 
                                        u32 size, uint protect);

    /*
     * Reserve and commit a memory section; initialize its content with provided
     * data
     */
    LxResult        AllocCopy       (   const SectionDesc &desc, u32 address, 
                                        u32 virtualSize, uint protect, 
                                        pbyte data, u32 size);

    /*
     * Commit a block of memory; must be in a single section
     */
    LxResult        Commit          (u32 address, u32 size, uint protect);

    /*
     * Reserve a block of memory
     */
    LxResult        Reserve         (const SectionDesc &desc, u32 address, u32 size, uint protect);

    /*
     * Decommit and release a section
     */
    LxResult        Free            (u32 address);

    /*
     * Decommit a block of memory; must be in a single section
     */
    LxResult        Decommit        (u32 address, u32 size);

    /*
     * Release a section; this section must be all reserved or all committed
     */
    LxResult        Release         (u32 address);

    /*
     * Creates a heap
     */
    Heap *          CreateHeap      (u32 base, u32 reserve, u32 commit, uint nModule = LX_UNKNOWN_MODULE);

    /*
     * Destroys a heap
     */
    bool            DestroyHeap     (Heap *heap);

    /*
     * Creates a stack
     */
    Stack *         CreateStack     (u32 base, u32 reserve, u32 commit, uint nModule = LX_UNKNOWN_MODULE);

    /*
     * Destroy a stack
     */
    bool            DestroyStack    (Stack *stack);

    /*
     * Find continuous empty pages
     */
    u32             FindFreePages   (u32 base, u32 size);

    /*
     * Find maximum continuous empty pages
     * return: actual size
     */
    u32             FindMaxFreePages        (u32 base, u32 maxSize);
    u32             FindMaxFreePagesReverse (u32 top, u32 maxSize);

    bool            Contains        (u32 address) const { return GetSection(address) != NULL; }
    bool            Overlaps        (u32 address, u32 size) const;
    INLINE pbyte    GetRawData      (u32 address) const;
    void            Erase           (void);
    void            Clear           (void);

    INLINE uint     GetPageState    (uint addr) const;
    INLINE LxResult Read8           (u32 address, u8p val) const;
    INLINE LxResult Read16          (u32 address, u16p val) const;
    INLINE LxResult Read32          (u32 address, u32p val) const;
    INLINE LxResult Read64          (u32 address, u64p val) const;
    INLINE LxResult Read128         (u32 address, u128p val) const;
    INLINE LxResult Write8          (u32 address, u8 val);
    INLINE LxResult Write16         (u32 address, u16 val);
    INLINE LxResult Write32         (u32 address, u32 val);
    INLINE LxResult Write64         (u32 address, const u64 &val);
    INLINE LxResult Write128        (u32 address, const u128 &val);
    std::vector<SectionInfo>    GetMemoryInfo() const;
    std::vector<PageInfo>       GetSectionInfo(u32 address) const;
    SectionDesc     GetSectionDesc(u32 address) const;
protected:
    Section *       GetSection(u32 address) const { return m_sectionTable[PAGE_NUM(address)]; }
    void            InsertSection(Section *sec);
    void            RemoveSection(Section *sec);
    bool            IsHeap(Section *sec) const { return m_heaps.find(sec) != m_heaps.end(); }
    bool            IsStack(Section *sec) const { return m_stacks.find(sec) != m_stacks.end(); }
private:
    Section *       m_sectionTable[LX_PAGE_COUNT];
    std::vector<Section *>   m_sections;
    std::set<Section *>  m_heaps;
    std::set<Section *>  m_stacks;
}; // class Memory

INLINE uint Memory::GetPageState(uint addr) const 
{
    Section *s = GetSection(addr);
    return s == NULL ? LX_CHR_FREE : s->GetPageState(addr);
}

INLINE LxResult Memory::Read8(u32 address, u8p val) const 
{ 
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    return GetSection(address)->Read8(address, val); 
}

INLINE LxResult Memory::Read16(u32 address, u16p val) const 
{ 
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    Assert(GetSection(address) == GetSection(address+1));
    return GetSection(address)->Read16(address, val); 
}

INLINE LxResult Memory::Read32(u32 address, u32p val) const 
{ 
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    Assert(GetSection(address) == GetSection(address+3));
    return GetSection(address)->Read32(address, val); 
}

INLINE LxResult Memory::Read64(u32 address, u64p val) const
{
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    Assert(GetSection(address) == GetSection(address+7));
    return GetSection(address)->Read64(address, val);
}

INLINE LxResult Memory::Read128(u32 address, u128p val) const
{
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    Assert(GetSection(address) == GetSection(address+15));
    return GetSection(address)->Read128(address, val);
}

INLINE LxResult Memory::Write8(u32 address, u8 val) 
{ 
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    return GetSection(address)->Write8(address, val); 
}

INLINE LxResult Memory::Write16(u32 address, u16 val) 
{ 
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    Assert(GetSection(address) == GetSection(address+1));
    return GetSection(address)->Write16(address, val); 
}

INLINE LxResult Memory::Write32(u32 address, u32 val) 
{ 
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    Assert(GetSection(address) == GetSection(address+3));
    return GetSection(address)->Write32(address, val); 
}

INLINE LxResult Memory::Write64(u32 address, const u64 &val)
{
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    Assert(GetSection(address) == GetSection(address+7));
    return GetSection(address)->Write64(address, val);
}

INLINE LxResult Memory::Write128(u32 address, const u128 &val)
{
    if (!Contains(address)) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    Assert(GetSection(address) == GetSection(address+15));
    return GetSection(address)->Write128(address, val);
}

INLINE pbyte Memory::GetRawData(u32 address) const 
{ 
    Assert(GetSection(address));
    return GetSection(address)->GetRawData(address); 
}

INLINE static u32 RoundUp(u32 size)
{
    return PAGE_LOW(size) == 0 ? size : PAGE_HIGH(size) + LX_PAGE_SIZE;
}

INLINE static u32 RoundDown(u32 size)
{
    return PAGE_HIGH(size);
}


END_NAMESPACE_LOCHSEMU()

#endif // __CORE_MEMORY_H__
