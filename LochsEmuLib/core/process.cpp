#include "stdafx.h"
#include "process.h"
#include "heap.h"
#include "memory.h"
#include "thread.h"
#include "emulator.h"
#include "pemodule.h"
#include "winapi.h"
#include "win32.h"
#include "processor.h"
#include "config.h"

BEGIN_NAMESPACE_LOCHSEMU()

Process::Process()
{
    m_emu           = NULL;
    m_memory        = NULL;
    m_loader        = NULL;
    m_PebAddress    = 0;
    m_plugins       = NULL;

    ZeroMemory(m_threads, sizeof(m_threads));
}

Process::~Process()
{
    for (int i = 0; i < MaximumThreads; i++)
        SAFE_DELETE(m_threads[i]);

    for (uint i = 0; i < m_heaps.size(); i++) {
        //SAFE_DELETE(m_heaps[i]);
        Mem()->DestroyHeap(m_heaps[i]);
    }
    m_heaps.clear();

    m_emu           = NULL;
    m_memory        = NULL;
    m_loader        = NULL;
    m_plugins       = NULL;
    m_PebAddress    = 0;
}

LochsEmu::LxResult Process::Initialize(Emulator *emu)
{
    Assert(emu);
    m_emu       = emu;
    m_memory    = emu->Mem();
    m_loader    = emu->Loader();
    m_plugins   = emu->Plugins();
    Assert(m_memory);
    Assert(m_loader);
    LxDebug("Initializing main emulated process\n");
    if (m_loader->GetNumOfModules() == 0) {
        return LX_RESULT_NOT_INITIALIZED;
    }
    
    LxResult lr;
    V_RETURN( InitHeap() );
    V_RETURN( InitPEB() );

    V_RETURN( InitMainThread() );

    LoadApiInfo();
    
    RET_SUCCESS();
}


LochsEmu::LxResult Process::InitHeap()
{
    LxDebug("Initializing main process Heap, reserve: 0x%x, commit: 0x%x\n",
        GetModuleInfo(0)->HeapReserve, GetModuleInfo(0)->HeapCommit);
    HeapID id = CreateHeap(GetModuleInfo(0)->HeapReserve, GetModuleInfo(0)->HeapCommit, 0);
    Assert((uint) id == ProcessHeapStart);
    RET_SUCCESS();
}

LochsEmu::LxResult Process::InitPEB()
{
    SyncObjectLock lock(*m_memory);

    LxDebug("Initializing main process PEB\n");
    WIN32_PEB *pPeb = GetPEBPtr();
    u32 BASE = Emu()->InquirePebAddress();
    const u32 size = sizeof(WIN32_PEB);
    m_PebAddress = Mem()->FindFreePages(BASE, size);
    Assert(m_PebAddress != 0);
    if (m_PebAddress != BASE) {
        LxWarning("PEB is loading at a unusual address: 0x%08x\n", m_PebAddress);
    }
    V( m_memory->AllocCopy(SectionDesc("PEB", LX_MAIN_MODULE), m_PebAddress, size, PAGE_READWRITE, (pbyte) pPeb, size) );
    WIN32_PEB *pProcessPeb = (WIN32_PEB *) m_memory->GetRawData(m_PebAddress);

    // TODO : refine PEB
    pProcessPeb->BeingDebugged = FALSE;
    pProcessPeb->ImageBaseAddress = (PVOID) GetModuleInfo(0)->ImageBase;

    RET_SUCCESS();
}

u32 Process::DetermineHeapBase(u32 base, u32 reserve)
{
    u32 b = Mem()->FindFreePages(base, reserve);
    Assert( b != 0 );
    return b;
}

HeapID Process::CreateHeap( u32 reserve, u32 commit, uint flags )
{
    SyncObjectLock lock(*m_memory);
    HeapID id = (HeapID) m_heaps.size() + ProcessHeapStart;

    uint base = LxConfig.GetUint("Process", "DefaultHeapBase", 0x1000000);
    uint minReserved = LxConfig.GetUint("Process", "MinimumHeapReserved", 0x1000000);    // 16MB
    if (reserve < minReserved) reserve = minReserved;

    base = DetermineHeapBase(base, reserve);
    Heap *newHeap = Mem()->CreateHeap(base, reserve, commit, LX_MAIN_MODULE);
    Assert(newHeap != NULL);
    m_heaps.push_back(newHeap);
    LxDebug("Heap created: base 0x%x, reserve 0x%x, commit 0x%x\n",
        base, reserve, commit);
    return id;
}


bool Process::DestroyHeap( HeapID id )
{
    Assert(id < m_heaps.size());
    Heap *heap = m_heaps[id - ProcessHeapStart];
    bool r = Mem()->DestroyHeap(heap);
    m_heaps[id] = NULL;
    LxDebug("Heap destroyed: %x\n", id);
    return r;
}


LochsEmu::LxResult Process::InitMainThread()
{
    const ThreadID ID = (ThreadID) 0; //GetCurrentThreadId();
    ThreadID realId = GetCurrentThreadId();
    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, realId);
    m_threads[ID]   = new Thread(this, realId, hThread);

    ThreadInfo info;
    info.EntryPoint = GetModuleInfo(0)->EntryPoint;
    info.Flags      = 0;
    info.ParamPtr   = 0;
    info.StackSize  = GetModuleInfo(0)->StackReserve;
    info.Module     = LX_MAIN_MODULE;

    LxResult lr;
    V_RETURN( m_threads[0]->Initialize(info) );

    CloseHandle(hThread);
    RET_SUCCESS();
}

LochsEmu::LxResult Process::Run()
{
    std::vector<uint>   moduleLoad;

    /* Run each DllMain */
    for (uint i = m_loader->GetNumOfModules() - 1; i >= 1; i--) {
        V( m_threads[0]->RunModuleEntry(i, LX_LOAD_PROCESS_ATTACH, LX_LOAD_STATIC) );
    }
    moduleLoad = m_threads[0]->GetModuleLoadOrder();
    Assert(moduleLoad.size() == m_loader->GetNumOfModules() - 1);

    /* Prolog */
    ProcessProlog();

    V( Plugins()->OnProcessPreRun(this, m_threads[0]->CPU()) );
    
    /* Run main thread */
    V( m_threads[0]->Run() );

    /* Epilog */
    ProcessEpilog();

    V( Plugins()->OnProcessPostRun(this) );

    /* Run each DllMain */
    for (int i = (int)moduleLoad.size() - 1; i >= 0; i--) {
        V( m_threads[0]->RunModuleEntry(moduleLoad[i], LX_LOAD_PROCESS_DETACH, LX_LOAD_STATIC) );
    }

    RET_SUCCESS();
}

Thread * Process::ThreadCreate( const ThreadInfo &ti )
{
    SyncObjectLock lock(*this);

    ThreadID id = FindNextThreadId();
    m_threads[id] = new Thread(this);

    V( m_threads[id]->Initialize(ti) );

    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) &LxThreadRoutine, 
        m_threads[id], ti.Flags, NULL);

    m_threads[id]->Handle   = hThread;
    m_threads[id]->ID = GetThreadId(hThread);
    return m_threads[id];
}

HMODULE Process::GetModule( LPCWSTR lpName )
{
    if (lpName == NULL) return GetModule((LPCSTR) NULL);
    char buf[128];
    int len = wcslen(lpName);
    LxWideToByte(lpName, buf, len);
    return GetModule(buf);
}

HMODULE Process::GetModule( LPCSTR lpName )
{
    if (lpName == NULL) {
        /* return self's image base */
        return (HMODULE) GetModuleInfo(0)->ImageBase;
    }
    for (uint i = 0; i < m_loader->GetNumOfModules(); i++) {
        if (!stricmp(lpName, GetModuleInfo(i)->Name)) {
            return (HMODULE) GetModuleInfo(i)->ImageBase;
        }
    }
    return LxGetModuleHandle(lpName);
}

u32 Process::GetProcAddr( HMODULE hModule, LPCSTR lpName )
{
    if (LX_IS_EMU_MODULE(hModule)) {
        // Emulated API
        uint idx = QueryWinAPIIndexByName(hModule, lpName);
        if (idx == 0) {
            LxWarning("Unsupported emulated API: %s::%s\n", LxGetModuleName(hModule), lpName);
            return 0;
        }
        return LX_MAKE_WINAPI_INDEX(idx);
    } else {
        for (uint i = 0; i < m_loader->GetNumOfModules(); i++) {
            if (GetModuleInfo(i)->ImageBase == (u32) hModule) {
                const ExportTable &table = GetModuleInfo(i)->Exports;
                for (uint idxFunc = 0; idxFunc < table.size(); idxFunc++) {
                    if (!stricmp(lpName, table[idxFunc].Name.c_str())) {
                        return table[idxFunc].Address;
                    }
                }
            }
        }
        LxWarning("Unknown API: HMODULE[0x%08x]::%s\n", (u32) hModule, lpName);
        return 0;
    }
}

uint Process::GetModuleFileName( HMODULE hModule, LPSTR lpBuffer, uint size )
{
    const char *lpName = NULL;
    if (hModule == NULL) {
        lpName = m_emu->Path();
    } else if (LX_IS_EMU_MODULE(hModule)) {
        lpName = LxGetModuleName(hModule);

    } else {
        for (uint i = 0; i < m_loader->GetNumOfModules(); i++) {
            if (hModule == (HMODULE) GetModuleInfo(i)->ImageBase) {
                lpName = GetModuleInfo(i)->Name;
            }
        }
    }
    Assert(lpName);
    uint len = min(strlen(lpName), size) + 1;
    strncpy(lpBuffer, lpName, len);
    return len;
}

ModuleInfo * Process::GetModuleInfoAddr( u32 addr )
{
    Section * sec = m_memory->GetSection(addr);
    if (sec == NULL) return NULL;
    return GetModuleInfo(sec->Module());
}

uint Process::GetModuleFileName( HMODULE hModule, LPWSTR lpBuffer, uint size )
{
    Assert(size < 0x4000);
    char buf[0x4000];
    uint len = this->GetModuleFileName(hModule, buf, size);
    LxByteToWide(buf, lpBuffer, len);
    return len;
}

uint Process::LoadModule( LPCSTR lpFileName )
{
    return m_loader->RuntimeLoad(lpFileName);
}

void Process::ProcessProlog()
{
    m_threads[0]->CPU()->Push32(0);
    m_threads[0]->CPU()->Push32(m_PebAddress);
}

void Process::ProcessEpilog()
{

}

u32 Process::GetEntryPoint() const
{
    return m_threads[0]->GetThreadInfo()->EntryPoint;
}

const LochsEmu::ApiInfo *Process::GetApiInfoFromAddress( u32 addr ) const
{
    auto iter = m_addressApiInfo.find(addr);
    if (iter == m_addressApiInfo.end()) { return NULL; }
    return &iter->second;
}

void Process::LoadApiInfo()
{
    uint nModules = m_loader->GetNumOfModules();
    for (uint i = 0; i < nModules; i++) {
        const ModuleInfo *moduleInfo = m_loader->GetModuleInfo(i);
        std::string moduleName = moduleInfo->Name;
        for (auto ex : moduleInfo->Exports) {
            ApiInfo info = { moduleName, ex.Name };
            m_addressApiInfo[ex.Address] = info;
        }

        for (auto import : moduleInfo->Imports) {
            const std::string &dllName = import.first;
            for (auto importFunc : import.second) {
                ApiInfo info = { dllName, importFunc.Name };
                m_addressApiInfo[importFunc.IATOffset] = info;
            }
        }
    }

    const int nEmuApis = LxGetTotalWinAPIs();
    for (int i = 0; i < nEmuApis; i++) {
        uint index = LX_MAKE_WINAPI_INDEX(i);
        ApiInfo info = { 
            LxGetWinAPIModuleName(index), LxGetWinAPIName(index)
        };
        m_addressApiInfo[index] = info;
    }
}

void Process::Terminate()
{
    m_threads[0]->CPU()->Terminate(0);
}

ThreadID Process::FindNextThreadId() const
{
    for (int i = 1; i < MaximumThreads; i++)
        if (m_threads[i] == NULL) return i;
    LxFatal("Cannot find valid thread ID\n");
    return -1;
}

Thread * Process::GetThreadRealID( ThreadID id ) const
{
    for (int i = 0; i < MaximumThreads; i++)
        if (m_threads[i] != NULL && m_threads[i]->ID == id)
            return m_threads[i];
    return NULL;
}


END_NAMESPACE_LOCHSEMU()