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
        m_pagetable[pageNum] = new PageTaint();
        //m_pagetable[pageNum] = m_pool.Alloc<PageTaint>();
    }
    return m_pagetable[pageNum];
}

void MemoryTaint::Reset()
{
    for (int i = 0; i < LX_PAGE_COUNT; i++) {
        if (m_pagetable[i] != NULL)
            m_pagetable[i]->Reset();
    }
//     for (int i = 0; i < LX_PAGE_COUNT; i++) {
//         m_pagetable[i] = NULL;
//     }
//     m_pool.Clear();
}

// void MemoryTaint::Init( const char *pool )
// {
//     m_pool.Init(pool);
// }

Taint MemoryTaint::PageTaint::Get( u32 offset )
{
    Assert(offset < LX_PAGE_SIZE);

    if (m_data[offset] == NULL)
        m_data[offset] = new Taint();

    //Taint t;
    //m_pool.Read(m_data[offset], &t);
    return *m_data[offset];
}

void MemoryTaint::PageTaint::Set( u32 offset, const Taint &t )
{
    Assert(offset < LX_PAGE_SIZE);

    if (m_data[offset] == 0)
        m_data[offset] = new Taint(t);
    else
        *(m_data[offset]) = t;

    //m_pool.Write(m_data[offset], &t);
}

void MemoryTaint::PageTaint::Reset()
{
    for (int i = 0; i < LX_PAGE_SIZE; i++)
        if (m_data[i]) SAFE_DELETE(m_data[i]);
}

MemoryTaint::PageTaint::PageTaint( )
    //: m_pool(pool)
{
    ZeroMemory(m_data, sizeof(m_data));
}

MemoryTaint::PageTaint::~PageTaint()
{
    Reset();
}

void ProcessorTaint::Reset()
{
    for (int i = 0; i < 8; i++) {
        GPRegs[i].ResetAll();
        MM[i].ResetAll();
        XMM[i].ResetAll();
    }
    for (int i = 0; i < InstContext::FLAG_COUNT; i++)
        Flags[i].ResetAll();
    Eip.ResetAll();
}
