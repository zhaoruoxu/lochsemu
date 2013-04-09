#include "stdafx.h"
#include "section.h"
#include "memory.h"

BEGIN_NAMESPACE_LOCHSEMU()


Section::Section( const SectionDesc &desc, u32 base, u32 size )
: m_desc(desc), m_base(base), m_size(size), m_pages(PAGE_NUM(size)), 
PhysAddress((u32) this)
{
    Assert(PAGE_LOW(base) == 0);
    Assert(PAGE_LOW(size) == 0);

    // Initialize all pages to NO_ACCESS & RESERVED
    m_pageDescTable = new PageDesc[m_pages];
    for (uint i = 0; i < m_pages; i++)
        m_pageDescTable[i] = PageDesc();

    // Allocate a block of memory, even reserved
    m_dataPtr = (pbyte) VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
}

Section::~Section()
{
    SAFE_DELETE_ARRAY(m_pageDescTable);
    if (m_dataPtr)
        Free();
}

LxResult Section::Commit( u32 addr, u32 size, uint protect )
{
    Assert(Contains(addr));
    Assert(addr + size <= m_base + m_size);

    u32 head = addr - m_base;
    u32 tail = addr - m_base + size - 1;
    for (uint n = PAGE_NUM(head); n <= PAGE_NUM(tail); n++) {
        SetPageDesc(n, protect, LX_CHR_COMMITTED);
    }
    LPVOID lpAddr = VirtualAlloc(m_dataPtr + (addr - m_base), size, MEM_COMMIT, PAGE_READWRITE);
    Assert(lpAddr == m_dataPtr + (addr - m_base));
    RET_SUCCESS();
    
}

LxResult Section::Decommit( u32 addr, u32 size )
{
    Assert(Contains(addr));
    Assert(addr + size <= m_base + m_size);

    u32 head = addr - m_base;
    u32 tail = addr - m_base + size - 1;
    for (uint n = PAGE_NUM(head); n <= PAGE_NUM(tail); n++) {
        SetPageDesc(n, PAGE_NOACCESS, LX_CHR_RESERVED);
    }
    B( VirtualFree(m_dataPtr + (addr - m_base), size, MEM_DECOMMIT) );
    RET_SUCCESS();
}

LxResult Section::Release()
{
    uint pageChr = m_pageDescTable[0].Characristics;
    for (uint i = 1; i < m_pages; i++) {
        if (m_pageDescTable[i].Characristics != pageChr) 
            return LX_RESULT_INVALID_OPERATION;
    }
    B( VirtualFree(m_dataPtr, 0, MEM_RELEASE) );
    m_dataPtr = NULL;
    RET_SUCCESS();
}

void Section::Free()
{
    B( VirtualFree(m_dataPtr, m_size, MEM_DECOMMIT) );
    B( VirtualFree(m_dataPtr, 0, MEM_RELEASE) );
    m_dataPtr = NULL;
}

void Section::Copy( u32 addr, u32 size, pbyte data )
{
    Assert(PAGE_LOW(addr) == 0);
    if (size > m_size) {
        LxWarning("Section copying from a larger source; size=%x m_size=%x",
            size, m_size);
    }
    for (uint i = PAGE_NUM(addr-m_base); i < PAGE_NUM(addr-m_base + RoundUp(size)); i++) {
        Assert(m_pageDescTable[i].Characristics & LX_CHR_COMMITTED);
    }
    memcpy(m_dataPtr + (addr - m_base), data, min(size, m_size));
}

std::vector<PageInfo> Section::GetSectionInfo() const
{
    std::vector<PageInfo> r;
    for (uint i = 0; i < m_pages; i++) {
        r.push_back(PageInfo(Base() + LX_PAGE_SIZE * i, m_pageDescTable[i]));
    }
    return r;
}   

END_NAMESPACE_LOCHSEMU()