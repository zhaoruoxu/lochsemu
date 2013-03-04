#include "stdafx.h"
#include "comptaint.h"


MemoryTaint::MemoryTaint() 
{
    ZeroMemory(m_pagetable, sizeof(m_pagetable));
}

MemoryTaint::~MemoryTaint()
{
    for (int i = 0; i < LX_PAGE_COUNT; i++) {
        if (m_pagetable[i]) {
            SAFE_DELETE(m_pagetable[i]);
        }
    }
}

MemoryTaint::PageTaint * MemoryTaint::GetPage( u32 addr )
{
    const u32 pageNum = PAGE_NUM(addr);
    if (m_pagetable[pageNum] == NULL) {
        m_pagetable[pageNum] = new PageTaint;
    }
    return m_pagetable[pageNum];
}

MemoryTaint::PageTaint::PageTaint()
{
    ZeroMemory(m_data, sizeof(m_data));
}

MemoryTaint::PageTaint::~PageTaint()
{

}

Taint MemoryTaint::PageTaint::Get( u32 offset )
{
    Assert(offset < LX_PAGE_SIZE);
    return m_data[offset];
}

void MemoryTaint::PageTaint::Set( u32 offset, const Taint &t )
{
    Assert(offset < LX_PAGE_SIZE);
    m_data[offset] = t;
}
