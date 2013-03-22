#include "stdafx.h"
#include "pemodule.h"
#include "memory.h"
#include "winapi.h"

BEGIN_NAMESPACE_LOCHSEMU()


PeModule::PeModule()
{
    m_data = NULL;
    m_dosHeader = NULL;
    m_ntHeaders = NULL;
    m_sectionheader = NULL;
    m_size = 0;
}

PeModule::PeModule( LPCSTR lpFileName )
{
    m_data = NULL;
    m_dosHeader = NULL;
    m_ntHeaders = NULL;
    m_sectionheader = NULL;
    m_size = 0;

    V( Load(lpFileName) );
}

PeModule::~PeModule()
{
    Unload();
}

LxResult PeModule::Load(LPCSTR path)
{
    strcpy(m_info.Name, LxFileName(path).c_str());
    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        return LX_RESULT_ERROR_OPEN_FILE;
    }

    DWORD fileSizeHigh;
    m_size = GetFileSize(hFile, &fileSizeHigh);
    if (fileSizeHigh > 0) {
        CloseHandle(hFile);
        return LX_RESULT_INVALID_FORMAT;
    }

    B( m_data = new byte[m_size] );
    DWORD nBytesRead;
    if (!ReadFile(hFile, m_data, m_size, &nBytesRead, NULL) ||
        nBytesRead != m_size) {
        CloseHandle(hFile);
        return LX_RESULT_ERROR_READ_FILE;
    }
    CloseHandle(hFile);
    return LoadData();
}

LxResult PeModule::LoadData()
{
    Assert(m_data);
    LxDebug("Validating headers\n");

    /*
     * Parses and validates headers
     */
    m_dosHeader = (PIMAGE_DOS_HEADER) m_data;
    if (m_dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return LX_RESULT_INVALID_FORMAT;
    }
    LxDebug("Found DOS magic\n");
    m_ntHeaders = (PIMAGE_NT_HEADERS)(m_data + m_dosHeader->e_lfanew);
    if (m_ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return LX_RESULT_INVALID_FORMAT;
    }
    LxDebug("Found NT magic\n");
    m_sectionheader = (PIMAGE_SECTION_HEADER) ((pbyte) m_ntHeaders + sizeof(IMAGE_NT_HEADERS));
    LxDebug("Valid PE module\n");

    /*
     * Loads module info
     */
    u32 imageBase = m_ntHeaders->OptionalHeader.ImageBase;
    m_info.LinearAddressIAT = m_ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress;
    m_info.LinearSizeIAT = m_ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size;
    m_info.OriginalImageBase = imageBase;
    m_info.EntryPoint = m_ntHeaders->OptionalHeader.AddressOfEntryPoint;
    m_info.StackReserve = m_ntHeaders->OptionalHeader.SizeOfStackReserve;
    m_info.StackCommit = m_ntHeaders->OptionalHeader.SizeOfStackCommit;
    m_info.HeapReserve = m_ntHeaders->OptionalHeader.SizeOfHeapReserve;
    m_info.HeapCommit = m_ntHeaders->OptionalHeader.SizeOfHeapCommit;
    memcpy(m_info.DataDirectory, m_ntHeaders->OptionalHeader.DataDirectory, 
        sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_NUMBEROF_DIRECTORY_ENTRIES);

    RET_SUCCESS();
}

uint PeModule::GetHeadersSize() const
{
    return GetDosHeader()->e_lfanew + sizeof(IMAGE_NT_HEADERS) + 
        sizeof(IMAGE_SECTION_HEADER) * GetNumOfSections();
}

void PeModule::Unload()
{
    SAFE_DELETE_ARRAY(m_data);
}

LochsEmu::ModuleInfo PeModule::GetModuleInfo( Memory *mem, u32 base )
{
    m_info.ImageBase = base;
    m_info.LinearAddressIAT += base;
    m_info.EntryPoint += base;

    m_info.Imports = GetImports(mem, base);
    m_info.Exports = GetExports(mem, base);
    m_info.Relocations = GetRelocations(mem, base);

    return m_info;
}

ImportTable PeModule::GetImports( Memory *mem, u32 imageBase )
{
    if (m_imports.size() > 0) return m_imports;

    PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)
        mem->GetRawData(imageBase + m_info.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    Assert(pImport);

    int idxDll = 0;
    while (true) {
        const IMAGE_IMPORT_DESCRIPTOR &import = pImport[idxDll];
        if (import.FirstThunk == 0 && import.Name == 0 && import.OriginalFirstThunk == 0) break;
        const char *dllName = (const char *) mem->GetRawData(imageBase + import.Name);
        Assert(dllName);
        std::vector<ImportEntry> funcs;
        PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA) mem->GetRawData(imageBase + import.FirstThunk);
        Assert(pThunk);
        DWORD origThunk = import.OriginalFirstThunk;
        PIMAGE_THUNK_DATA pOrigThunk = NULL;
        if (0 == origThunk) {
            pOrigThunk = pThunk;
        } else {
            pOrigThunk = (PIMAGE_THUNK_DATA) mem->GetRawData(imageBase + origThunk);
        }
        Assert(pOrigThunk); 

        int idxFunc = 0;
        while (true) {
            if (pOrigThunk[idxFunc].u1.AddressOfData == 0) break;
            if (pOrigThunk[idxFunc].u1.AddressOfData & IMAGE_ORDINAL_FLAG) {
                // import by ordinal
                int ord = IMAGE_ORDINAL(pOrigThunk[idxFunc].u1.AddressOfData);
                LxWarning("Importing by ordinal, DLL: %s, ordinal: %d\n", dllName, ord);
                funcs.push_back(ImportEntry(ord, 
                    import.FirstThunk + idxFunc * sizeof(IMAGE_THUNK_DATA) + imageBase));
            } else {
                // import by name
                const char *funcName = (const char *) mem->GetRawData(imageBase + pOrigThunk[idxFunc].u1.AddressOfData + 2);
                Assert(funcName);
                funcs.push_back(ImportEntry(funcName, import.FirstThunk + idxFunc * sizeof(IMAGE_THUNK_DATA) + imageBase));
            }
            ++idxFunc;
        }
        m_imports[std::string(dllName)] = funcs;
        ++idxDll;
    }

    return m_imports;
}

ExportTable PeModule::GetExports( Memory *mem, u32 imageBase )
{
    if (m_exports.size() > 0) return m_exports;

    u32 exportRva = m_info.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportRva == 0) { /* No export */ return m_exports; }

    PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)
        mem->GetRawData(imageBase + exportRva);
    Assert(pExport);

    const char *dllName = (const char *) mem->GetRawData(imageBase + pExport->Name);
    if (stricmp(dllName, m_info.Name)) {
        LxWarning("DLL name conflicts in export directory: original[%s] export[%s]\n",
            m_info.Name, dllName);
    }
    uint base = pExport->Base;
    uint numNames = pExport->NumberOfNames;
    u32p pFunctions = (u32p) mem->GetRawData(imageBase + pExport->AddressOfFunctions);
    u32p pNames = (u32p) mem->GetRawData(imageBase + pExport->AddressOfNames);
    u16p pOrdinals = (u16p) mem->GetRawData(imageBase + pExport->AddressOfNameOrdinals);

    bool hasZeroOrdinal = false;
    for (uint idxName = 0; idxName < numNames; idxName++) {
        const char *funcName = (const char *) mem->GetRawData(imageBase + pNames[idxName]);
        Assert(funcName);

        u16 ordinal = pOrdinals[idxName];
        u32 address = pFunctions[ordinal] + imageBase;
        if (ordinal == 0) hasZeroOrdinal = true;

        m_exports.push_back(ExportEntry(std::string(funcName), address, ordinal));
    }

    if (hasZeroOrdinal) {
        LxWarning("%s has zero ordinal, fixing\n", dllName);
        for (auto &exp : m_exports) 
            exp.Ordinal++;
    }
    
    return m_exports;
}

#define REL_TYPE(x)     ((x & 0xf000) >> 12)

RelocationTable PeModule::GetRelocations( Memory *mem, u32 imageBase )
{
    if (m_relocs.size() > 0) return m_relocs;

    u32 relocRva = m_info.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    if (relocRva == 0) { /* No relocations */ return m_relocs; }

    PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION) mem->GetRawData(imageBase + relocRva);
    while (true) {
        u16p reloc = (u16p) mem->GetRawData(imageBase + relocRva + sizeof(IMAGE_BASE_RELOCATION));
        uint size = (pReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
        for (uint i = 0; i < size; i++) {
            u16 r = reloc[i];
            if (REL_TYPE(r) == IMAGE_REL_BASED_ABSOLUTE) {
                // do nothing
            } else if (REL_TYPE(r) == IMAGE_REL_BASED_HIGHLOW) {
                u32 relPos = (r & 0x0fff) + pReloc->VirtualAddress + imageBase;
                m_relocs.push_back(RelocationEntry(IMAGE_REL_BASED_HIGHLOW, relPos));
            } else {
                LxFatal("Unsupported relocation type: %x\n", REL_TYPE(r));
            }
        }
        relocRva += pReloc->SizeOfBlock;
        pReloc = (PIMAGE_BASE_RELOCATION) mem->GetRawData(imageBase + relocRva);
        if (pReloc->VirtualAddress == 0) break;
    }
    return m_relocs;
}


END_NAMESPACE_LOCHSEMU()