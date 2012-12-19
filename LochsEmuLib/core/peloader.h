#pragma once

#ifndef __CORE_PELOADER_H__
#define __CORE_PELOADER_H__

#include "lochsemu.h"
#include "pemodule.h"

BEGIN_NAMESPACE_LOCHSEMU()

/*
 * Load PE modules into memory
 */
class LX_API PeLoader {
public:
    PeLoader();
    virtual ~PeLoader();

    LxResult            Initialize(Emulator *emu);
    LxResult            Load(LPCSTR lpFileName);
    Emulator *          Emu() { Assert(m_emu); return m_emu; }
    uint                RuntimeLoad(LPCSTR lpFileName);
    ModuleInfo *        GetModuleInfo(uint n) { Assert(n < m_infos.size()); return &m_infos[n]; }
    const ModuleInfo *  GetModuleInfo(uint n) const { Assert(n < m_infos.size()); return &m_infos[n]; }
    const uint          GetNumOfModules() const { return m_infos.size(); }
    uint                GetModuleIndex(LPCSTR name);

protected:
    /*
     * Loads an pe module, exe or dll
     */
    LxResult            LoadModule(LPCSTR lpFileName);

    /*
     * Loads dependent DLLs for the nth module
     */
    LxResult            LoadLibraries(uint nModule);

    /*
     * Load IAT, relocations and stuff
     */
    LxResult            PostProcess();

    /*
     * Load IAT for module number n
     */
    LxResult            LoadIAT(uint nModule);

    /*
     * Fixup relocations
     */
    LxResult            Relocate(uint nModule);

    /*
     * A module may not be loaded at its ImageBase
     * Its image base is determined by PeLoader
     */
    u32                 DetermineImageBase(PeModule *module);
    LxResult            LoadSectionToMem(PeModule *module, PIMAGE_SECTION_HEADER pSec, u32 base, uint nModule);
    uint                GetModuleExportIndexByName(uint nModule, const char *funcName);
    uint                GetModuleExportIndexByOrdinal(uint nModule, uint ordinal);
protected:
    Emulator *          m_emu;
    Memory *            m_memory;
    Process *           m_process;
    RefProcess *        m_refProcess;
    std::string         m_fileName;
    std::vector<ModuleInfo> m_infos;
};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_PELOADER_H__
