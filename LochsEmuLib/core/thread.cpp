#include "stdafx.h"
#include "thread.h"
#include "processor.h"
#include "memory.h"
#include "process.h"
#include "stack.h"
#include "winapi.h"
#include "emulator.h"

BEGIN_NAMESPACE_LOCHSEMU()

Thread::Thread( Process *proc, int intId, ThreadID extId, HANDLE hThread)
: ExtID(extId), IntID(intId), Handle(hThread), m_cpu(intId, this)
{
    Assert(proc);
    m_process       = proc;
    m_memory        = proc->Mem();
    m_plugins       = proc->Plugins();
    m_stack         = NULL; 
    m_TebAddress    = 0;
    ExitCode        = 0;
}

Thread::~Thread()
{
    B( Mem()->DestroyStack(m_stack) );
    m_memory    = NULL;
    m_plugins   = NULL;
}

LochsEmu::LxResult Thread::Initialize(const ThreadInfo &info)
{
    LxDebug("Initializing Thread ID[%x]\n", ExtID);
    memcpy(&m_initInfo, &info, sizeof(ThreadInfo));

    LxResult lr;
    InitStack();
    InitTEB();
    V_RETURN( m_cpu.Initialize() );

    RET_SUCCESS();
}

void Thread::InitStack()
{
    SyncObjectLock lock(*m_memory);

    if (m_initInfo.StackSize == 0)
        m_initInfo.StackSize = Proc()->Emu()->InquireStackBase() - 
        Proc()->Emu()->InquireStackLimit();
    u32 base = Proc()->Emu()->InquireStackBase() - m_initInfo.StackSize;

    base = m_memory->FindFreePages(base, m_initInfo.StackSize);

    LxInfo("Initializing Stack for thread[%x], base %08x, size 0x%x\n", ExtID, 
        base, m_initInfo.StackSize);
    m_stack = Mem()->CreateStack(base, m_initInfo.StackSize, m_initInfo.StackSize, 
        m_initInfo.Module);
    if (m_stack == NULL) 
        LxFatal("Create stack failed\n");
}

void Thread::InitTEB()
{
    SyncObjectLock lock(*m_memory);

    LxDebug("Initializing TEB for thread[%x]\n", ExtID);
    WIN32_TEB *pTeb = GetTEBPtr();
    // current TEB is initialized on top of stack
    const u32 size = sizeof(WIN32_TEB);
    m_TebAddress = m_memory->FindFreePages(m_stack->Top(), size);
    Assert(m_TebAddress != 0);
    LxDebug("Allocating memory for TEB at [0x%08x]\n", m_TebAddress);
    char desc[64];
    sprintf(desc, "TEB (%x)", ExtID);
    V( m_memory->AllocCopy(SectionDesc(desc, m_initInfo.Module), m_TebAddress, size, PAGE_READWRITE, (pbyte) pTeb, size));
    WIN32_TEB *pThreadPeb = (WIN32_TEB *) m_memory->GetRawData(m_TebAddress);
    pThreadPeb->TIB.StackBase = m_stack->Top();
    pThreadPeb->TIB.StackLimit = m_stack->Bottom();
    pThreadPeb->TIB.Self = m_TebAddress;
    pThreadPeb->PEB_Ptr = m_process->GetPEBAddress();
    // TODO : refine teb
}


LochsEmu::LxResult Thread::Run()
{
    V( m_cpu.Run(m_initInfo.EntryPoint) );
    RET_SUCCESS();
}

LochsEmu::LxResult Thread::RunAt( u32 entry )
{
    RET_NOT_IMPLEMENTED();
}

LochsEmu::LxResult Thread::RunModuleEntry( uint nModule, LoadReason reason, LoadMethod method )
{
    /*
     * Calling DllMain
     */
    ModuleInfo *info = Proc()->GetModuleInfo(nModule);
    Assert(info);
    if (reason == LX_LOAD_PROCESS_ATTACH) {
        /*
         * Initial load, initialize modules
         */
        if (info->Initialized) RET_SUCCESS();
        /*
        * Run DLL's dependencies
        */
        ImportTable::const_iterator iter = info->Imports.begin();
        for (; iter != info->Imports.end(); iter++) {
            const char *moduleName = iter->first.c_str();
            if (IsEmulatedLibrary(moduleName)) continue;
            uint moduleIndex = Proc()->Loader()->GetModuleIndex(moduleName);
            if (moduleIndex == (uint) -1) {
                LxFatal("Unloaded module: %s\n", moduleName);
            }
            if (!Proc()->GetModuleInfo(moduleIndex)->Initialized) {
                V( RunModuleEntry(moduleIndex, reason, method) );
            }
        }

        /*
         * Record module load order
         */
        m_moduleLoadOrder.push_back(nModule);
    }

    m_cpu.Push32((u32) method);    // LPVOID lpvReserved
    m_cpu.Push32((u32) reason);    // DWORD fdwReason
    m_cpu.Push32(info->ImageBase); // HINSTANCE hinstDLL, = base address of DLL
    m_cpu.Push32(TERMINATE_EIP);   // Termination condition

    LxInfo("Calling %s::DllMain(hInstance=%x, reason=%d, reserved=%d)\n",
        info->Name, info->ImageBase, reason, method);
    V( m_cpu.RunConditional(info->EntryPoint) );
    BOOL okay =  m_cpu.EAX;
    if (!okay) { RET_FAIL(LX_RESULT_ERROR_LOAD_MODULE); }

    info->Initialized = true;
    RET_SUCCESS();
}

HMODULE Thread::LoadModule( LPCSTR lpFileName )
{
    uint nModule = Proc()->LoadModule(lpFileName);
    V( RunModuleEntry(nModule, LX_LOAD_PROCESS_ATTACH, LX_LOAD_DYNAMIC) );
    return (HMODULE) Proc()->GetModuleInfo(nModule)->ImageBase;
}

LochsEmu::LxResult Thread::UnloadModule( HMODULE hModule )
{
    if (LX_IS_EMU_MODULE(hModule)) {
        LxWarning("Trying to free an emulated module: %s\n", LxGetModuleName(hModule));
    }
    LxWarning("!! TODO : UnloadModule !!\n");
    RET_SUCCESS();
}

void Thread::Exit( u32 code )
{
    m_cpu.Terminate(code);
}

DWORD LxThreadRoutine( LPVOID lpParams )
{
    Thread *t = (Thread *) lpParams;

    t->CPU()->Push32(t->GetThreadInfo()->ParamPtr);
    t->CPU()->Push32((u32) TERMINATE_EIP);
    t->Run();
    u32 code = t->ExitCode;

    ThreadID id = GetCurrentThreadId();
    Assert(id == t->ExtID);

    LxEmulator.Plugins()->OnThreadExit(t);
    LxEmulator.Proc()->ThreadDelete(id);

    return code;
}

END_NAMESPACE_LOCHSEMU()