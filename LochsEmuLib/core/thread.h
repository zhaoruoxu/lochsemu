#pragma once

#ifndef __CORE_THREAD_H__
#define __CORE_THREAD_H__

#include "lochsemu.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

struct ThreadInfo {
    u32 StackSize;
    u32 EntryPoint;
    u32 ParamPtr;
    uint Flags;
    uint Module;

    ThreadInfo() {
        StackSize = EntryPoint = ParamPtr = Flags = 0;
        Module = LX_UNKNOWN_MODULE;
    }
};

enum LoadMethod {
    LX_LOAD_DYNAMIC = NULL,
    LX_LOAD_STATIC  = !(NULL),
};

enum LoadReason {
    LX_LOAD_PROCESS_DETACH  = DLL_PROCESS_DETACH,
    LX_LOAD_PROCESS_ATTACH  = DLL_PROCESS_ATTACH,
    LX_LOAD_THREAD_ATTACH   = DLL_THREAD_ATTACH,
    LX_LOAD_THREAD_DETACH   = DLL_THREAD_DETACH,
};

class LX_API Thread {
public:
    Thread(Process *proc, ThreadID id = 0, HANDLE hThread = INVALID_HANDLE_VALUE);
    virtual ~Thread();

    LxResult        Initialize(const ThreadInfo &info);
    LxResult        Run();
    LxResult        RunAt(u32 entry);
    HMODULE         LoadModule(LPCSTR lpFileName);
    LxResult        UnloadModule(HMODULE hModule);
    LxResult        RunModuleEntry(uint nModule, LoadReason reason, LoadMethod method);

    Stack *         GetStack() const { Assert(m_stack); return m_stack; }
    Memory *        Mem() const { Assert(m_memory); return m_memory; }
    Process *       Proc() const { Assert(m_process); return m_process; }
    const Processor *     CPU() const { return &m_cpu; }
    Processor *     CPU() { return &m_cpu; }
    PluginManager * Plugins() const { Assert(m_plugins); return m_plugins; }
    u32             GetTEBAddress() const { return m_TebAddress; }
    std::vector<uint>   GetModuleLoadOrder() const { return m_moduleLoadOrder; }
    const ThreadInfo *  GetThreadInfo() const { return &m_initInfo; }
public:
    ThreadID        ID;
    HANDLE          Handle;
protected:
    void            InitStack();
    void            InitTEB();
protected:
    Processor       m_cpu;
    Process *       m_process;
    Memory *        m_memory;
    PluginManager * m_plugins;
    ThreadInfo      m_initInfo;
    Stack *         m_stack;
    u32             m_TebAddress;
    std::vector<uint>   m_moduleLoadOrder;  /* Record module load order for module unloading */

    //LxResult        m_runResult;
};

DWORD LxThreadRoutine(LPVOID lpParams);

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_THREAD_H__
