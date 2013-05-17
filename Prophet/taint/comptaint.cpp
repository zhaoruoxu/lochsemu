#include "stdafx.h"
#include "comptaint.h"


MemoryTaint::MemoryTaint()
{
    ZeroMemory(m_pagetable, sizeof(m_pagetable));
}

MemoryTaint::~MemoryTaint()
{
    Reset();
}

MemoryTaint::PageTaint * MemoryTaint::GetPage( u32 addr )
{
    const u32 pageNum = PAGE_NUM(addr);
    if (m_pagetable[pageNum] == NULL) {
        m_pagetable[pageNum] = new PageTaint;
    }
    return m_pagetable[pageNum];
}

void MemoryTaint::Reset()
{
    for (int i = 0; i < LX_PAGE_COUNT; i++) {
        SAFE_DELETE(m_pagetable[i]);
    }
}

MemoryTaint * MemoryTaint::Clone() const
{
    MemoryTaint *t = new MemoryTaint;
    for (int i = 0; i < LX_PAGE_COUNT; i++) {
        if (m_pagetable[i]) t->m_pagetable[i] = m_pagetable[i]->Clone();
    }
    return t;
}

void MemoryTaint::CopyFrom( const MemoryTaint *t )
{
    for (int i = 0; i < LX_PAGE_COUNT; i++) {
        if (t->m_pagetable[i]) {
            if (m_pagetable[i]) 
                m_pagetable[i]->CopyFrom(t->m_pagetable[i]);
            else
                m_pagetable[i] = t->m_pagetable[i]->Clone();
        } else {
            SAFE_DELETE(m_pagetable[i]);
        }
    }
}


Taint MemoryTaint::PageTaint::Get( u32 offset )
{
    Assert(offset < LX_PAGE_SIZE);

    if (m_data[offset] == NULL)
        m_data[offset] = new Taint;

    return *m_data[offset];
}

void MemoryTaint::PageTaint::Set( u32 offset, const Taint &t )
{
    Assert(offset < LX_PAGE_SIZE);

    if (m_data[offset] == 0)
        m_data[offset] = new Taint(t);
    else
        *(m_data[offset]) = t;
}

void MemoryTaint::PageTaint::Reset()
{
    for (int i = 0; i < LX_PAGE_SIZE; i++)
        if (m_data[i]) SAFE_DELETE(m_data[i]);
}

MemoryTaint::PageTaint::PageTaint( )
{
    ZeroMemory(m_data, sizeof(m_data));
}

MemoryTaint::PageTaint::~PageTaint()
{
    Reset();
}

MemoryTaint::PageTaint * MemoryTaint::PageTaint::Clone() const
{
    PageTaint *t = new PageTaint;
    for (int i = 0; i < LX_PAGE_SIZE; i++) {
        if (m_data[i]) t->m_data[i] = new Taint(*m_data[i]);
    }
    return t;
}

void MemoryTaint::PageTaint::CopyFrom( const PageTaint *t )
{
    for (int i = 0; i < LX_PAGE_SIZE; i++) {
        if (t->m_data[i]) {
            if (m_data[i]) *(m_data[i]) = *(t->m_data[i]);
            else m_data[i] = new Taint(*t->m_data[i]);
        } else {
            SAFE_DELETE(m_data[i]);
        }
    }
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

ProcessorTaint * ProcessorTaint::Clone() const
{
    ProcessorTaint *t = new ProcessorTaint;
    memcpy(t, this, sizeof(ProcessorTaint));
    return t;
}

void ProcessorTaint::CopyFrom( const ProcessorTaint *t )
{
    memcpy(this, t, sizeof(ProcessorTaint));
}
