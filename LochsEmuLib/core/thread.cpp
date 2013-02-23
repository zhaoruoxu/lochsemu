#include "stdafx.h"
#include "thread.h"
#include "processor.h"
#include "memory.h"
#include "process.h"
#include "stack.h"
#include "winapi.h"
#include "emulator.h"

BEGIN_NAMESPACE_LOCHSEMU()

Thread::Thread( ThreadID id, Process *proc )
: THREAD_ID(id)
{
    Assert(proc);
    m_process       = proc;
    m_memory        = proc->Mem();
    m_plugins       = proc->Plugins();
    m_cpu           = NULL;
    m_stack         = NULL; 
    m_TebAddress    = 0;
    //m_runResult     = LX_RESULT_THREAD_FAILED;
}

Thread::~Thread()
{
    SAFE_DELETE(m_cpu);
    B( Mem()->DestroyStack(m_stack) );
    m_memory    = NULL;
    m_plugins   = NULL;
}

LochsEmu::LxResult Thread::Initialize(const ThreadInfo &info)
{
    LxDebug("Initializing Thread ID[%x]\n", THREAD_ID);
    memcpy(&m_initInfo, &info, sizeof(ThreadInfo));

    LxResult lr;
    B( m_cpu = new Processor(this) );
    V_RETURN( InitStack() );
    V_RETURN( InitTEB() );
    V_RETURN( m_cpu->Initialize() );

    RET_SUCCESS();
}

LochsEmu::LxResult Thread::InitStack()
{
    u32 base = Proc()->Emu()->InquireStackBase();
    u32 actualSize = m_memory->FindMaxFreePagesReverse(base, m_initInfo.StackSize);
    LxInfo("Allocated stack size: %08x, reserved: %08x\n", actualSize, m_initInfo.StackSize);

    LxDebug("Initializing Stack for thread[%x], size 0x%x\n", THREAD_ID, actualSize);
    m_stack = Mem()->CreateStack(base - actualSize, actualSize, actualSize, m_initInfo.Module);
    if (m_stack == NULL) return LX_RESULT_ERROR_INIT_STACK;
    RET_SUCCESS();
}

LochsEmu::LxResult Thread::InitTEB()
{
    LxDebug("Initializing TEB for thread[%x]\n", THREAD_ID);
    WIN32_TEB *pTeb = GetTEBPtr();
    // current TEB is initialized on top of stack
    const u32 size = sizeof(WIN32_TEB);
    m_TebAddress = m_memory->FindFreePages(m_stack->Top(), size);
    Assert(m_TebAddress != 0);
    LxDebug("Allocating memory for TEB at [0x%08x]\n", m_TebAddress);
    char desc[64];
    sprintf(desc, "TEB (%x)", THREAD_ID);
    V( m_memory->AllocCopy(SectionDesc(desc, m_initInfo.Module), m_TebAddress, size, PAGE_READWRITE, (pbyte) pTeb, size));
    WIN32_TEB *pThreadPeb = (WIN32_TEB *) m_memory->GetRawData(m_TebAddress);
    pThreadPeb->TIB.StackBase = m_stack->Top();
    pThreadPeb->TIB.StackLimit = m_stack->Bottom();
    pThreadPeb->TIB.Self = m_TebAddress;
    // TODO : refine teb

    pThreadPeb->PEB_Ptr = m_process->GetPEBAddress();
    
    RET_SUCCESS();
}


LochsEmu::LxResult Thread::Run()
{
    V( m_cpu->Run(m_initInfo.EntryPoint) );
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
    Assert(m_cpu);
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

    m_cpu->Push32((u32) method);    // LPVOID lpvReserved
    m_cpu->Push32((u32) reason);    // DWORD fdwReason
    m_cpu->Push32(info->ImageBase); // HINSTANCE hinstDLL, = base address of DLL
    m_cpu->Push32(TERMINATE_EIP);   // Termination condition

    LxInfo("Calling %s::DllMain(hInstance=%x, reason=%d, reserved=%d)\n",
        info->Name, info->ImageBase, reason, method);
    V( m_cpu->RunConditional(info->EntryPoint) );
    BOOL okay =  m_cpu->EAX;
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

END_NAMESPACE_LOCHSEMU()