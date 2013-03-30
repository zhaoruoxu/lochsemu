#pragma once

#ifndef __CORE_PROCESS_H__
#define __CORE_PROCESS_H__

#include "lochsemu.h"
#include "peloader.h"
#include "emulator.h"
#include "thread.h"
#include "parallel.h"

BEGIN_NAMESPACE_LOCHSEMU()

struct ApiInfo {
    std::string ModuleName;
    std::string FunctionName;
};

class LX_API Process : public MutexSyncObject {
public:
    static const uint   ProcessHeapStart = 0x1000;
    static const int    MaximumThreads = 256;

public:
    Process();
    virtual ~Process();

    Memory *        Mem() const { Assert(m_memory); return m_memory; }
    Emulator *      Emu() const { Assert(m_emu); return m_emu; }
    PeLoader *      Loader() const { Assert(m_loader); return m_loader; }
    PluginManager * Plugins() const { Assert(m_plugins); return m_plugins; }

    LxResult        Initialize(Emulator *emu);
    LxResult        Run();
    void            Terminate();

    Thread *        ThreadCreate(const ThreadInfo &ti);

    HeapID          CreateHeap(u32 reserve, u32 commit, uint flags);
    bool            DestroyHeap(HeapID id);
    Thread *        GetThread(ThreadID id) const { Assert(id < MaximumThreads); return m_threads[id]; }
    Thread *        GetThreadRealID(ThreadID id) const;
    Heap *          GetHeap(uint id) const { return m_heaps[id - ProcessHeapStart]; }
    u32             GetPEBAddress() const { return m_PebAddress; }
    HMODULE         GetModule(LPCSTR lpName);
    HMODULE         GetModule(LPCWSTR lpName);
    uint            GetModuleFileName(HMODULE hModule, LPSTR lpBuffer, uint size);
    uint            GetModuleFileName(HMODULE hModule, LPWSTR lpBuffer, uint size);
    u32             GetProcAddr(HMODULE hModule, LPCSTR lpName);
    ModuleInfo *    GetModuleInfo(uint n) { return m_loader->GetModuleInfo(n); }
    const ModuleInfo *  GetModuleInfo(uint n) const { return m_loader->GetModuleInfo(n); }
    ModuleInfo *    GetModuleInfoAddr(u32 addr);
    uint            LoadModule(LPCSTR lpFileName);
    u32             GetEntryPoint() const;
    const ApiInfo * GetApiInfoFromAddress(u32 addr) const;
protected:
    LxResult        InitHeap();
    LxResult        InitPEB();
    LxResult        InitMainThread();
    u32             DetermineHeapBase(u32 base, u32 reserve);
    void            ProcessProlog();
    void            ProcessEpilog();
    void            LoadApiInfo();
    ThreadID        FindNextThreadId() const;

protected:
    Emulator *      m_emu;
    Memory *        m_memory;
    PeLoader *      m_loader;
    PluginManager * m_plugins;

    Thread *        m_threads[MaximumThreads];  // main thread is always m_threads[0]
    //std::vector<Thread *>   m_threads;
    //std::map<ThreadID, Thread *>     m_threads;
    //Thread *        m_mainThread;       /* also contained in m_threads */
    std::vector<Heap *>     m_heaps; /* [0] is process main heap */
    u32             m_PebAddress;

    /*
     * 各模块导出函数地址与模块名称和函数名称的映射
     */
    std::map<u32, ApiInfo> m_addressApiInfo;  
};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_PROCESS_H__
