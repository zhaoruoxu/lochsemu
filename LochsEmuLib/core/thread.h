#pragma once

#ifndef __CORE_THREAD_H__
#define __CORE_THREAD_H__

#include "lochsemu.h"
#include "process.h"

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

class LX_API Thread {
public:
    Thread(ThreadID id, Process *proc);
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
    Processor *     CPU() const { Assert(m_cpu); return m_cpu; }
    PluginManager * Plugins() const { Assert(m_plugins); return m_plugins; }
    u32             GetTEBAddress() const { return m_TebAddress; }
    std::vector<uint>   GetModuleLoadOrder() const { return m_moduleLoadOrder; }
    const ThreadInfo *  GetThreadInfo() const { return &m_initInfo; }
public:
    const ThreadID  THREAD_ID;
protected:
    LxResult        InitStack();
    LxResult        InitTEB();
protected:
    Processor *     m_cpu;
    Process *       m_process;
    Memory *        m_memory;
    PluginManager * m_plugins;
    ThreadInfo      m_initInfo;
    Stack *         m_stack;
    u32             m_TebAddress;
    std::vector<uint>   m_moduleLoadOrder;  /* Record module load order for module unloading */
};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_THREAD_H__
