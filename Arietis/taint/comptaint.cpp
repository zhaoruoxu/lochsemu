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

// Taint MemoryTaint::Get( u32 addr )
// {
//     PageTaint *page = GetPage(addr);
//     return page->Get(PAGE_LOW(addr));
// }
// 
// Taint MemoryTaint::Get( u32 addr, u32 len )
// {
//     Taint ret = Get(addr);
//     for (uint i = 1; i < len; i++) {
//         ret |= Get(addr + i);
//     }
//     return ret;
// }
// 
// void MemoryTaint::Set( const Taint &t, u32 addr, u32 len )
// {
//     Assert(len > 0);
//     for (uint i = 0; i < len; i++) {
//         //m_status[addr + i] = t;
//         PageTaint *page = GetPage(addr);
//         page->Set(PAGE_LOW(addr + i), t);
//     }
// }

MemoryTaint::PageTaint * MemoryTaint::GetPage( u32 addr )
{
    const u32 pageNum = PAGE_NUM(addr);
    if (m_pagetable[pageNum] == NULL) {
        m_pagetable[pageNum] = new PageTaint;
    }
    return m_pagetable[pageNum];
}

Taint MemoryTaint::Get8( u32 addr )
{
    PageTaint *page = GetPage(addr);
    return page->Get(PAGE_LOW(addr));
}

Taint16 MemoryTaint::Get16( u32 addr )
{
    PageTaint *page0 = GetPage(addr); 
    PageTaint *page1 = GetPage(addr+1);
    return PackTaint(page0->Get(PAGE_LOW(addr)), page1->Get(PAGE_LOW(addr+1)));
}

Taint32 MemoryTaint::Get32( u32 addr )
{
    Taint32 res;
    for (int i = 0; i < Taint32::Count; i++) {
        res.T[i] = GetPage(addr+i)->Get(PAGE_LOW(addr+i));
    }
    return res;
}

void MemoryTaint::Set8( u32 addr, const Taint &t )
{
    GetPage(addr)->Set(PAGE_LOW(addr), t);
}

void MemoryTaint::Set16( u32 addr, const Taint16 &t )
{
    GetPage(addr  )->Set(PAGE_LOW(addr),   t.T[0]);
    GetPage(addr+1)->Set(PAGE_LOW(addr+1), t.T[1]);
}

void MemoryTaint::Set32( u32 addr, const Taint32 &t )
{
    for (int i = 0; i < Taint32::Count; i++) {
        GetPage(addr+i)->Set(PAGE_LOW(addr+i), t.T[i]);
    }
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
