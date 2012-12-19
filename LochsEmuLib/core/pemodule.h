#pragma once

#ifndef __CORE_PEMODULE_H__
#define __CORE_PEMODULE_H__

#include "lochsemu.h"
#include "pemodule.h"

BEGIN_NAMESPACE_LOCHSEMU()

enum PeModuleType {
    LX_PE_MODULE_EXE,
    LX_PE_MODULE_DLL,
};

/*
 * An import entry
 */
struct ImportEntry {
    std::string Name;
    u32 IATOffset;
    u32 Ordinal;

    ImportEntry(const std::string &name, u32 iatOffset)
        : Name(name), IATOffset(iatOffset), Ordinal((u32) -1)
    {
    }

    ImportEntry(u32 ordinal, u32 iatOffset)
        : Name(""), Ordinal(ordinal), IATOffset(iatOffset)
    {
    }
};

typedef std::map<std::string, std::vector<ImportEntry> > ImportTable;

/*
 * An export entry
 */
struct ExportEntry {
    std::string Name;
    u32 Address;
    u16 Ordinal;

    ExportEntry(const std::string &name, u32 addr, u16 ord)
        : Name(name), Address(addr), Ordinal(ord)
    {
    }
};

typedef std::vector<ExportEntry> ExportTable;

/*
 * A relocation entry
 */
struct RelocationEntry {
    u32 Type;
    u32 Address;

    RelocationEntry(u32 t, u32 a) : Type(t), Address(a) {}
};

typedef std::vector<RelocationEntry> RelocationTable;

/*
 * Stores runtime info of a module
 */
struct LX_API ModuleInfo {
    u32 LinearAddressIAT;
    u32 LinearSizeIAT;
    u32 ImageBase;
    u32 OriginalImageBase;
    u32 EntryPoint;
    u32 StackReserve;
    u32 StackCommit;
    u32 HeapReserve;
    u32 HeapCommit;
    char Name[MAX_PATH];
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
    ImportTable Imports;
    ExportTable Exports;
    RelocationTable Relocations;
    bool Initialized;

    ModuleInfo() {
        LinearAddressIAT = LinearSizeIAT = 0;
        OriginalImageBase = ImageBase = EntryPoint = 0;
        StackReserve = StackCommit = HeapReserve = HeapCommit = 0;
        Initialized = false;
        ZeroMemory(Name, MAX_PATH);
        ZeroMemory(DataDirectory, sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_NUMBEROF_DIRECTORY_ENTRIES);
    }
};


class LX_API PeModule {
    // An PE module: exe or dll
public:
    PeModule();
    PeModule(LPCTSTR lpFileName);
    virtual ~PeModule();
    
    LxResult    Load(LPCTSTR path);
    void        Unload();

public:
    LPCSTR GetName() const { return m_info.Name; }
    PIMAGE_DOS_HEADER GetDosHeader() const { 
        Assert(m_dosHeader); return m_dosHeader; 
    }
    PIMAGE_NT_HEADERS GetNtHeader() const { 
        Assert(m_ntHeaders); return m_ntHeaders; 
    }
    PIMAGE_FILE_HEADER GetFileHeader() const { 
        Assert(m_ntHeaders); return &m_ntHeaders->FileHeader; 
    }
    PIMAGE_OPTIONAL_HEADER GetOptionalHeader() const {
        Assert(m_ntHeaders); return &m_ntHeaders->OptionalHeader;
    }
    uint GetNumOfSections() const {
        Assert(m_ntHeaders); return m_ntHeaders->FileHeader.NumberOfSections;
    }
    PIMAGE_SECTION_HEADER GetSectionHeaderBase() const { 
        Assert(m_sectionheader); return m_sectionheader; 
    }
    PIMAGE_SECTION_HEADER GetSectionHeader(uint n) const {
        Assert(n < GetNumOfSections()); return &m_sectionheader[n];
    }
    pbyte GetDataPtr(uint offset = 0) const {
        Assert(offset < m_size);
        return m_data + offset;
    }
    uint            GetHeadersSize() const;
    uint            GetImageSize() const { Assert(m_size > 0); return m_size; }
    ModuleInfo      GetModuleInfo(Memory *mem, u32 base);
    ImportTable     GetImports(Memory *mem, u32 imageBase);
    ExportTable     GetExports(Memory *mem, u32 imageBase);
    RelocationTable GetRelocations(Memory *mem, u32 imageBase);

protected:
    LxResult LoadData();
protected:
    pbyte m_data;
    uint m_size;
    ModuleInfo m_info;
    ImportTable m_imports;
    ExportTable m_exports;
    RelocationTable m_relocs;

    PIMAGE_DOS_HEADER m_dosHeader;
    PIMAGE_NT_HEADERS m_ntHeaders;
    PIMAGE_SECTION_HEADER m_sectionheader;
};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_PEMODULE_H__
