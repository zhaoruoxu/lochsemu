#include "stdafx.h"
#include "peloader.h"
#include "pemodule.h"
#include "emulator.h"
#include "memory.h"
#include "process.h"
#include "winapi.h"
#include "refproc.h"

BEGIN_NAMESPACE_LOCHSEMU()

static uint ImageToPageProtect(uint p)
{
    // TODO: 
    return PAGE_READWRITE;
}

PeLoader::PeLoader()
{
    m_emu           = NULL;
    m_memory        = NULL;
    m_process       = NULL;
    m_refProcess    = NULL;
}

PeLoader::~PeLoader()
{
    m_emu           = NULL;
    m_memory        = NULL;
    m_process       = NULL;
    m_refProcess    = NULL;
}


LochsEmu::LxResult PeLoader::Initialize( Emulator *emu )
{
    Assert(emu);
    m_emu           = emu;
    m_memory        = emu->Mem();
    m_process       = emu->Proc();
    m_refProcess    = emu->RefProc();
    Assert(m_memory);
    Assert(m_process);

    RET_SUCCESS();
}

LochsEmu::LxResult PeLoader::Load( LPCSTR lpFileName )
{
    LxResult lr;
    m_fileName = lpFileName;
    V_RETURN( LoadModule(lpFileName) );

    V_RETURN( LoadLibraries(0) );

    V_RETURN( PostProcess() );

    RET_SUCCESS();
}

LochsEmu::LxResult PeLoader::LoadModule( LPCSTR lpFileName )
{
    LxInfo("Loading module: %s\n", lpFileName);
    PeModule module;
    V( module.Load(lpFileName) );

    SyncObjectLock lock(*m_memory);

    /*
     * First load each section to memory
     */
    u32 imageBase = DetermineImageBase(&module);
    uint nSections = module.GetNumOfSections();
    LxInfo("Loading module [%s, %d sections] at base 0x%08x\n", lpFileName, nSections, imageBase);

    /* PE header to section 0 */
    u32 size = module.GetHeadersSize();
    char desc[64];
    sprintf(desc, "PE header (%s)", module.GetName());
    V( m_memory->AllocCopy(SectionDesc(desc, m_infos.size()), imageBase, size, PAGE_READONLY, 
        module.GetDataPtr(), size) );

    for (uint i = 0; i < nSections; i++) {
        V( LoadSectionToMem(&module, module.GetSectionHeader(i), imageBase, m_infos.size()) );
    }

    /*
     * Then save module info(IAT/stack/heap/etc.) for future load
     */
    ModuleInfo info = module.GetModuleInfo(m_memory, imageBase);
    m_infos.push_back(info);

    LxDebug("\n");
    
    RET_SUCCESS();
}


uint PeLoader::RuntimeLoad( LPCSTR lpFileName )
{
    V( LoadModule(lpFileName) );

    const uint nModule = m_infos.size() - 1;
    V( LoadLibraries(nModule) );

    V( LoadIAT(nModule) );
    if (m_infos[nModule].OriginalImageBase != m_infos[nModule].ImageBase) {
        V( Relocate(nModule) );
    }
    return nModule;
}


LochsEmu::LxResult PeLoader::LoadLibraries(uint nModule)
{
    Assert(nModule < m_infos.size());
    LxDebug("Loading dependencies for %s\n", m_infos[nModule].Name);

    ImportTable import = m_infos[nModule].Imports;
    ImportTable::iterator iter = import.begin();
    for (; iter != import.end(); iter++) {
        std::string dllName = iter->first;
        if (IsEmulatedLibrary(dllName.c_str())) continue;
        if (GetModuleIndex(dllName.c_str()) != (uint) -1) continue;

        // other dlls, should be loaded
        // may process path of dll
        RefProcess::ModuleLoadInfo *info = m_refProcess->GetModuleLoadInfo(dllName.c_str());
        std::string dllPath = dllName;
        if (!LxFileExists(dllName.c_str())) {
            // search from suggested module info
            if (info) {
                dllPath = info->Path;
            }
            if (!LxFileExists(dllPath.c_str())) {
                dllPath = LxFilePath(m_fileName.c_str()) + dllName;
                if (!LxFileExists(dllPath.c_str())) {
                    dllPath = LxModulePath(dllName.c_str()) + dllName;
                    if (!LxFileExists(dllPath.c_str())) {
                        LxFatal("Dependent DLL not exist: %s\n", dllName.c_str());
                    }
                }
            }
        }

        V( LoadModule(dllPath.c_str()) );
        uint index = m_infos.size() - 1;    // newly loaded module is always the last one
        V( LoadLibraries(index) );
    }

    LxDebug("\n");
    RET_SUCCESS();
}


LochsEmu::LxResult PeLoader::LoadSectionToMem( PeModule *module, PIMAGE_SECTION_HEADER pSec, u32 base, uint nModule )
{
    u32 addr = base + pSec->VirtualAddress;
    uint protect = ImageToPageProtect(pSec->Characteristics);
    LxResult lr;

    LxDebug("Loading section:%s [0x%08x]\tVirtualSize:%x\tVirtualAddress:%x\t"
        "RawSize:%x\tDataPtr:%x\n", pSec->Name, addr, pSec->Misc.VirtualSize,
        pSec->VirtualAddress, pSec->SizeOfRawData, pSec->PointerToRawData);

    u32 size = max(pSec->Misc.VirtualSize, pSec->SizeOfRawData);
    char desc[64];
    sprintf(desc, "%s", pSec->Name);

    SectionDesc sdesc(desc, nModule);
    if (pSec->PointerToRawData == 0) {
        V( lr = m_memory->Alloc(sdesc, addr, size, protect) );
    } else {
        V( lr = m_memory->AllocCopy(sdesc, addr, size, protect, 
            module->GetDataPtr(pSec->PointerToRawData), pSec->SizeOfRawData) );
    }
    return lr;
}

u32 PeLoader::DetermineImageBase( PeModule *module )
{
    LPCSTR name = module->GetName();
    RefProcess::ModuleLoadInfo *info = m_refProcess->GetModuleLoadInfo(name);
    if (info) return info->ImageBase;

    u32 base = module->GetOptionalHeader()->ImageBase;
    if (base == 0) 
        base = 0x10000000;
    // TODO: determine total number of pages to use
    return m_memory->FindFreePages(base, LX_PAGE_SIZE); 
}

LochsEmu::LxResult PeLoader::PostProcess()
{
    Assert(m_infos.size() > 0);

    for (uint i = 0; i < m_infos.size(); i++) {
        V( LoadIAT(i) );
    }

    for (uint i = 0; i < m_infos.size(); i++) {
        if ( m_infos[i].OriginalImageBase != m_infos[i].ImageBase ) {
            V( Relocate(i) );
        }
    }

    RET_SUCCESS();
}

LochsEmu::LxResult PeLoader::LoadIAT( uint nModule )
{
    const ModuleInfo &info = m_infos[nModule];

    ImportTable::const_iterator importIter = info.Imports.begin();
    for (; importIter != info.Imports.end(); importIter++) {

        const char *dllName = importIter->first.c_str();

        if (IsEmulatedLibrary(dllName)) {
            // Load from API Hooking table
            for (uint i = 0; i < importIter->second.size(); i++) {
                uint index;
                if (importIter->second[i].Ordinal != (uint) -1) {
                    // import by ordinal in emulated library
                    uint ord = importIter->second[i].Ordinal;
                    index = QueryWinAPIIndexByOrdinal(dllName, ord);
                } else {
                    const char *funcName = importIter->second[i].Name.c_str();
                    index = QueryWinAPIIndexByName(dllName, funcName);

                }
                V( m_memory->Write32(importIter->second[i].IATOffset, LX_MAKE_WINAPI_INDEX(index)) );
            }
            continue;
        } 
        
        uint dllIndex = GetModuleIndex(dllName);
        if (dllIndex == (uint) -1) {
            LxFatal("DLL [%s] not loaded\n", dllName); return LX_RESULT_ERROR_LOAD_MODULE;
        }

        bool ordinalNeedFix = false;
        for (uint i = 0; i < importIter->second.size(); i++) {
            uint index;
            if (importIter->second[i].Ordinal != (uint) -1) {
                // import by ordinal
                index = GetModuleExportIndexByOrdinal(dllIndex, importIter->second[i].Ordinal);
                if (index == (uint) -1) {
                    const ModuleInfo &dll = m_infos[dllIndex];
                    LxWarning("Ordinal need fixing! dll = %s, ord = %d(%x)\n",
                        dll.Name, importIter->second[i].Ordinal, importIter->second[i].Ordinal);
                    ordinalNeedFix = true;
                    break;
                }
            } else {
                const char *funcName = importIter->second[i].Name.c_str();
                index = GetModuleExportIndexByName(dllIndex, funcName);
                if (index == (uint) -1) {
                    LxFatal("Exported function [%s] not found in [%s]\n", funcName, dllName);
                    return LX_RESULT_ERROR_LOAD_MODULE;
                }
            }

            const ExportEntry &entry = m_infos[dllIndex].Exports[index];
//             LxDebug("Loading function in library %s::%s (%d)0x%08x\n", dllName, entry.Name.c_str(),
//                 entry.Ordinal, entry.Address);
            V( m_memory->Write32(importIter->second[i].IATOffset, entry.Address) );
        }

        if (ordinalNeedFix) {
            for (uint i = 0; i < importIter->second.size(); i++) {
                uint index;
                if (importIter->second[i].Ordinal != (uint) -1) {
                    // import by ordinal
                    index = GetModuleExportIndexByOrdinal(dllIndex, importIter->second[i].Ordinal - 1);
                    if (index == (uint) -1) {
                        LxFatal("Exported function [ordinal: 0x%x] not found in [%s]\n", importIter->second[i].Ordinal, dllName);
                        return LX_RESULT_ERROR_LOAD_MODULE;
                    }
                } else {
                    const char *funcName = importIter->second[i].Name.c_str();
                    index = GetModuleExportIndexByName(dllIndex, funcName);
                    if (index == (uint) -1) {
                        LxFatal("Exported function [%s] not found in [%s]\n", funcName, dllName);
                        return LX_RESULT_ERROR_LOAD_MODULE;
                    }
                }
                const ExportEntry &entry = m_infos[dllIndex].Exports[index];
                V( m_memory->Write32(importIter->second[i].IATOffset, entry.Address) );
            }
        }
    }

    RET_SUCCESS();
}


LochsEmu::LxResult PeLoader::Relocate( uint nModule )
{
    const ModuleInfo &info = m_infos[nModule];

    LxDebug("Fixing up relocations for module %s\n", info.Name);

    const u32 offset = info.ImageBase - info.OriginalImageBase;
    u32 temp;
    for (uint i = 0; i < info.Relocations.size(); i++) {
        V( m_memory->Read32(info.Relocations[i].Address, &temp) );
        //LxDebug("At 0x%x, 0x%x relocates to 0x%x\n", info.Relocations[i].Address, temp, temp + offset);
        V( m_memory->Write32(info.Relocations[i].Address, temp + offset));
    }

    LxDebug("\n");
    RET_SUCCESS();
}


uint PeLoader::GetModuleIndex( LPCSTR name )
{
    for (uint i = 0; i < m_infos.size(); i++)
        if (!stricmp(name, m_infos[i].Name)) return i;
    return (uint) -1;
}

uint PeLoader::GetModuleExportIndexByName( uint nModule, const char *funcName )
{
    Assert(nModule < m_infos.size());
    for (uint i = 0; i < m_infos[nModule].Exports.size(); i++)
        if (!strcmp(funcName, m_infos[nModule].Exports[i].Name.c_str())) return i;
    return (uint) -1;
}

uint PeLoader::GetModuleExportIndexByOrdinal( uint nModule, uint ordinal )
{
    Assert(nModule < m_infos.size());
    for (uint i = 0; i < m_infos[nModule].Exports.size(); i++)
        if (m_infos[nModule].Exports[i].Ordinal == ordinal) return i;
    return (uint) -1;
}

END_NAMESPACE_LOCHSEMU()