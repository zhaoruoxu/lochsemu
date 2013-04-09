#include "stdafx.h"
#include "memory.h"
#include "section.h"
#include "heap.h"
#include "stack.h"

BEGIN_NAMESPACE_LOCHSEMU()

LX_API Memory::Memory()
{
    ZeroMemory(m_sectionTable, sizeof(m_sectionTable));
}


LX_API Memory::~Memory()
{
    ZeroMemory(m_sectionTable, sizeof(Section *) * LX_PAGE_COUNT);
    for (uint i = 0; i < m_sections.size(); i++) {
        SAFE_DELETE(m_sections[i]);
    }
    m_sections.clear();
}

LX_API LxResult Memory::Alloc( const SectionDesc &desc, u32 address, u32 size, uint protect )
{
    LxResult lr = Reserve(desc, address, size, protect);
    if (LX_FAILED(lr)) return lr;
    return Commit(address, size, protect);
}

LX_API LxResult Memory::AllocCopy( const SectionDesc &desc, u32 address, u32 virtualSize, uint protect, pbyte data, u32 size )
{
    LxResult lr = Reserve(desc, address, virtualSize, protect);
    if (LX_FAILED(lr)) return lr;
    lr = Commit(address, virtualSize, protect);
    if (LX_FAILED(lr)) return lr;
    GetSection(address)->Copy(address, size, data);
    RET_SUCCESS();
}

LX_API LxResult Memory::Commit( u32 address, u32 size, uint protect )
{
    u32 actualAddr = RoundDown(address);
    u32 actualSize = RoundUp(address + size) - actualAddr;

    Section *sec = GetSection(address);
    if (sec == NULL) {
        // Cannot commit free pages
        return LX_RESULT_INVALID_OPERATION;
    }
    if (GetSection(actualAddr + actualSize - 1) != sec) {
        // Exceed boundary
        return LX_RESULT_INVALID_OPERATION;
    }
    if (IsHeap(sec) || IsStack(sec)) {
        // Trying to commit in a heap or a stack
        return LX_RESULT_INVALID_OPERATION;
    }
    return sec->Commit(actualAddr, actualSize, protect);
}

LX_API LxResult Memory::Reserve( const SectionDesc &desc, u32 address, u32 size, uint protect )
{
    u32 actualAddr = RoundDown(address);
    u32 actualSize = RoundUp(address + size) - actualAddr;

    Section *sec = GetSection(address);
    if (sec != NULL) {
        // Cannot reserve an existing section
        return LX_RESULT_INVALID_OPERATION;
    }
    // Create a new section
    if (Overlaps(actualAddr, actualSize)) 
        return LX_RESULT_INVALID_OPERATION;
    sec = new Section(desc, actualAddr, actualSize);
    InsertSection(sec);
    RET_SUCCESS();
}

LX_API LxResult Memory::Free( u32 address )
{
    Section *sec = GetSection(address);
    if (sec == NULL) return LX_RESULT_INVALID_OPERATION;
    if (sec->Base() != address) 
        return LX_RESULT_INVALID_OPERATION;
    // cannot free a heap or stack
    if (IsHeap(sec) || IsStack(sec)) return LX_RESULT_INVALID_OPERATION;
    sec->Free();
    RemoveSection(sec);
    RET_SUCCESS();
}

LX_API LxResult Memory::Decommit( u32 address, u32 size )
{
    u32 actualAddr = RoundDown(address);
    u32 actualSize = RoundUp(address + size) - actualAddr;

    Section *sec = GetSection(address);
    if (sec == NULL) {
        // Decommitting free pages
        return LX_RESULT_INVALID_OPERATION;
    }
    if (GetSection(actualAddr + actualSize - 1) != sec) {
        // Decommitting exceed boundary
        return LX_RESULT_INVALID_OPERATION;
    }
    if (IsHeap(sec) || IsStack(sec)) {
        // cannot decommit in a heap or a stack
        return LX_RESULT_INVALID_OPERATION;
    }
    return sec->Decommit(actualAddr, actualSize);
}

LX_API LxResult Memory::Release( u32 address ) 
{
    if (PAGE_LOW(address) != 0) return LX_RESULT_INVALID_ADDRESS;
    Section *sec = GetSection(address);
    if (sec == NULL) return LX_RESULT_INVALID_OPERATION;
    if (sec->Base() != address) return LX_RESULT_INVALID_OPERATION;
    // cannot release a heap or a stack
    if (IsHeap(sec) || IsStack(sec)) return LX_RESULT_INVALID_OPERATION;
    LxResult lr = sec->Release();
    if (LX_FAILED(lr)) return lr;
    RemoveSection(sec);
    RET_SUCCESS();
}


LX_API Heap * Memory::CreateHeap( u32 base, u32 reserve, u32 commit, uint nModule )
{
    Assert(PAGE_LOW(base) == 0);
    Assert(PAGE_LOW(reserve) == 0);

    if (Overlaps(base, reserve)) return NULL;
    Heap *heap = new Heap(base, reserve, commit, nModule);
    InsertSection(heap);
    m_heaps.insert(heap);
    return heap;
}

LX_API bool Memory::DestroyHeap( Heap *heap )
{
    Assert(heap);

    if (m_heaps.find(heap) == m_heaps.end())
        return false;
    m_heaps.erase(heap);
    RemoveSection(heap);
    return true;
}


LX_API Stack * Memory::CreateStack( u32 base, u32 reserve, u32 commit, uint nModule )
{
    Assert(PAGE_LOW(base) == 0);
    Assert(PAGE_LOW(reserve) == 0);

    if (Overlaps(base, reserve)) return NULL;
    Stack *stack = new Stack(base, reserve, commit, nModule);
    InsertSection(stack);
    m_stacks.insert(stack);
    return stack;
}

LX_API bool Memory::DestroyStack( Stack *stack )
{
    Assert(stack);

    if (m_stacks.find(stack) == m_stacks.end())
        return false;
    m_stacks.erase(stack);
    RemoveSection(stack);
    return true;
}

LX_API bool Memory::Overlaps( u32 address, u32 size ) const
{
    Assert(PAGE_LOW(address) == 0);
    Assert(PAGE_LOW(size) == 0);

    uint pageBegin = PAGE_NUM(address);
    uint pageEnd = PAGE_NUM(address + size);
    for (uint i = pageBegin; i < pageEnd; i++) {
        if (m_sectionTable[i] != NULL) return true;
    }
    return false;
}
 

void Memory::InsertSection( Section *sec )
{
    uint pageIdx = PAGE_NUM(sec->Base());
    uint pageCnt = PAGE_NUM(sec->Size());
    for (uint i = pageIdx; i < pageIdx+pageCnt; i++) {
        Assert(m_sectionTable[i] == NULL);
        m_sectionTable[i] = sec;
    }
    m_sections.push_back(sec);
}


void Memory::RemoveSection( Section *sec )
{
    std::vector<Section *>::iterator iter;
    iter = find(m_sections.begin(), m_sections.end(), sec);
    Assert(iter != m_sections.end());
    uint pageIdx = PAGE_NUM((*iter)->Base());
    uint pageCnt = PAGE_NUM((*iter)->Size());
    for (uint i = 0; i < pageCnt; i++)
        m_sectionTable[i+pageIdx] = NULL;
    SAFE_DELETE(*iter);
    m_sections.erase(iter);
}

LX_API void Memory::Erase()
{
    for (uint i = 0; i < m_sections.size(); i++)
        m_sections[i]->Erase();
}

LX_API void Memory::Clear()
{
    ZeroMemory(m_sectionTable, sizeof(Section *) * LX_PAGE_COUNT);
    for (uint i = 0; i < m_sections.size(); i++) {
        SAFE_DELETE(m_sections[i]);
    }
    m_sections.clear();
}

LX_API u32 Memory::FindFreePages( u32 base, u32 size )
{
    u32 actualSize = RoundUp(size);
    u64 addr = base;
    const u64 maxAddr = 0x100000000;
    u32 contSize = 0x0;
    while (addr < maxAddr) {
        Section *s = GetSection((u32) addr);
        if (s == NULL) {
            contSize += LX_PAGE_SIZE;
            addr += LX_PAGE_SIZE;
            if (contSize >= actualSize) {
                return (u32) addr - actualSize;
            }
        } else {
            contSize = 0;
            addr += max(s->Size(), LX_ALLOC_GRAN);
        }
    }
    return 0;
}   
// 
// u32 Memory::FindMaxFreePages( u32 base, u32 maxSize )
// {
//     u32 actualSize = RoundUp(maxSize);
//     u32 addr = base;
//     while (addr < base + actualSize) {
//         Section *s = GetSection(addr + LX_PAGE_SIZE);
//         if (s) break;
//         addr += LX_PAGE_SIZE;
//     }
//     return addr - base;
// }
// 
// 
// u32 Memory::FindMaxFreePagesReverse( u32 top, u32 maxSize )
// {
//     u32 actualSize = RoundUp(maxSize);
//     u32 addr = top - LX_PAGE_SIZE;
//     while (addr > top - maxSize) {
//         Section *s = GetSection(addr);
//         if (s) break;
//         addr -= LX_PAGE_SIZE;
//     }
//     return top - addr;
// }

std::vector<SectionInfo> Memory::GetMemoryInfo() const
{
    std::vector<SectionInfo> r;
    for (uint i = 0; i < m_sections.size(); i++) {
        r.push_back(SectionInfo(m_sections[i]->GetDesc(), m_sections[i]->Base(), m_sections[i]->Size()));
    }
    return r;
}

std::vector<PageInfo> Memory::GetSectionInfo(u32 address) const
{
    return GetSection(address)->GetSectionInfo();
}

LochsEmu::SectionDesc Memory::GetSectionDesc( u32 address ) const
{
    Section *section = GetSection(address);
    Assert(section);
    return section->GetDesc();
}

bool Memory::PhysToEmulated( u32 phys, u32p emulated )
{
    for (auto &sec : m_sections) {
        if (phys >= sec->PhysAddress && phys < sec->PhysAddress + sec->Size()) {
            *emulated = sec->Base() + (phys - sec->PhysAddress);
            return true;
        }
    }
    return false;
}



END_NAMESPACE_LOCHSEMU()