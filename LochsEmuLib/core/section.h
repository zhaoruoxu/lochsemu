#pragma once

#ifndef __CORE_SECTION_H__
#define __CORE_SECTION_H__

#include "lochsemu.h"
#include "memdesc.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define     LX_CHR_EMU_MEMORY       0x00000001
#define     LX_CHR_MAP_MEMORY       0x00000002

/*
 * Initially all pages in a section are reserved
 * A section should be allocated in a continuous block of memory
 */

class LX_API Section {
public:
    Section(const SectionDesc &desc, u32 base, u32 size);
    virtual ~Section();

    /*
     * Commit pages in a section
     * Committing committed pages will not fail
     */
    LxResult    Commit(u32 addr, u32 size, uint protect);

    /*
     * Decommit pages in a section
     * Decommitting reserved pages will not fail
     */
    LxResult    Decommit(u32 addr, u32 size);

    /*
     * Release the whole section
     */
    LxResult    Release();

    /*
     * Decommit and release a section
     */
    void        Free();

    /*
     * Utilities
     */
    void            Copy(u32 addr, u32 size, pbyte data);
    u32             Base() const { return m_base; }
    u32             Size() const { return m_size; }
    SectionDesc     GetDesc() const { return m_desc; }
    const std::string & Description() const { return m_desc.Desc; }
    uint            Module() const { return m_desc.Module; }
    bool            Contains(u32 addr) const { return IN_RANGE(addr, m_base, m_size); }
    INLINE bool     IsCommitted(uint pageNum) const;
    INLINE bool     IsAllCommitted() const;
    INLINE pbyte    GetRawData(u32 addr) const;
    INLINE void     Erase();
    INLINE uint     GetPageState(u32 addr) const;
    std::vector<PageInfo>    GetSectionInfo() const;

    INLINE LxResult Read8(u32 address, u8p val) const;
    INLINE LxResult Read16(u32 address, u16p val) const;
    INLINE LxResult Read32(u32 address, u32p val) const;
    INLINE LxResult Read64(u32 address, u64p val) const;
    INLINE LxResult Read128(u32 address, u128p val) const;
    INLINE LxResult Write8(u32 address, u8 value);
    INLINE LxResult Write16(u32 address, u16 value);
    INLINE LxResult Write32(u32 address, u32 value);
    INLINE LxResult Write64(u32 address, const u64 &value);
    INLINE LxResult Write128(u32 address, const u128 &value);

protected:
    INLINE void     SetPageDesc(uint pageNum, uint protect, uint chr);
    INLINE bool     CanRead(uint pageNum) const;
    INLINE bool     CanWrite(uint pageNum) const;
    INLINE bool     CanExecute(uint pageNum) const;

protected:
    SectionDesc     m_desc;
    u32             m_base;
    u32             m_size;
    u32             m_pages;
    PageDesc *      m_pageDescTable;
    pbyte           m_dataPtr;
};


INLINE bool Section::IsCommitted(uint pageNum) const { 
    return (m_pageDescTable[pageNum].Characristics & LX_CHR_COMMITTED) != 0;
}
INLINE pbyte Section::GetRawData(u32 addr) const { 
    //Assert(IsCommitted(PAGE_NUM(addr-m_base))); 
    Assert(Contains(addr)); 
    return m_dataPtr + (addr - m_base); 
}
INLINE void Section::Erase() {
    Assert(IsAllCommitted()); ZeroMemory(m_dataPtr, m_size); 
}
INLINE uint Section::GetPageState(u32 addr) const { 
    Assert(Contains(addr)); return m_pageDescTable[PAGE_NUM(addr-m_base)].Characristics; 
}

INLINE bool Section::IsAllCommitted() const
{
    for (uint i = 0; i < m_pages; i++) {
        if (!(m_pageDescTable[i].Characristics & LX_CHR_COMMITTED))
            return false;
    }
    return true;
}

INLINE void Section::SetPageDesc( uint pageNum, uint protect, uint chr )
{
    Assert(pageNum < PAGE_NUM(m_size));
    m_pageDescTable[pageNum].Protect = protect;
    m_pageDescTable[pageNum].Characristics = chr;
}


INLINE bool Section::CanRead( uint pageNum ) const
{
    const uint P = m_pageDescTable[pageNum].Protect;
    return (P != PAGE_NOACCESS) && (P != PAGE_EXECUTE);
}

INLINE bool Section::CanWrite( uint pageNum ) const
{
    const uint P = m_pageDescTable[pageNum].Protect;
    return (P == PAGE_READWRITE) || (P == PAGE_EXECUTE_READWRITE) || 
        (P == PAGE_WRITECOPY) || (P == PAGE_EXECUTE_WRITECOPY);
}

INLINE bool Section::CanExecute( uint pageNum ) const
{
    const uint P = m_pageDescTable[pageNum].Protect;
    return (P == PAGE_EXECUTE_READ) || (P == PAGE_EXECUTE) || 
        (P == PAGE_EXECUTE_READWRITE) || (P == PAGE_EXECUTE_WRITECOPY);
}

INLINE LxResult Section::Read8( u32 address, u8p val ) const
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanRead(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    *val = *((u8p) (m_dataPtr + (address - m_base)));
    RET_SUCCESS();
}

INLINE LxResult Section::Read16( u32 address, u16p val ) const
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanRead(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    *val = *((u16p) (m_dataPtr + (address - m_base)));
    RET_SUCCESS();
}

INLINE LxResult Section::Read32( u32 address, u32p val ) const
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanRead(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    *val = *((u32p) (m_dataPtr + (address - m_base)));
    RET_SUCCESS();
}

INLINE LxResult Section::Read64( u32 address, u64p val ) const
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanRead(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    *val = *((u64p) (m_dataPtr + (address - m_base)));
    RET_SUCCESS();
}

INLINE LxResult Section::Read128( u32 address, u128p val ) const
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanRead(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    memcpy(val, m_dataPtr + (address - m_base), sizeof(u128));
    RET_SUCCESS();
}

INLINE LxResult Section::Write8( u32 address, u8 value )
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanWrite(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    *((u8p) (m_dataPtr + (address - m_base))) = value;
    RET_SUCCESS();
}

INLINE LxResult Section::Write16( u32 address, u16 value )
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanWrite(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    *((u16p) (m_dataPtr + (address - m_base))) = value;
    RET_SUCCESS();
}

INLINE LxResult Section::Write32( u32 address, u32 value )
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanWrite(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    *((u32p) (m_dataPtr + (address - m_base))) = value;
    RET_SUCCESS();
}

INLINE LxResult Section::Write64( u32 address, const u64 &value )
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanWrite(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    *((u64p) (m_dataPtr + (address - m_base))) = value;
    RET_SUCCESS();
}

INLINE LxResult Section::Write128(u32 address, const u128 &value)
{
    Assert(Contains(address));
    if (!IsCommitted(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    if (!CanWrite(PAGE_NUM(address-m_base))) RET_FAIL(LX_RESULT_ACCESS_VIOLATION);
    memcpy(m_dataPtr + (address - m_base), &value, sizeof(u128));
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_SECTION_H__
