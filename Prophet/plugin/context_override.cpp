#include "stdafx.h"
#include "context_override.h"
#include "event.h"
#include "process.h"
#include "emulator.h"
#include "refproc.h"
#include "thread.h"
#include "stack.h"

ContextOverride::ContextOverride( ProPluginManager *manager )
    : Plugin(manager, true, "ContextOverride", PreExecuteHandler)
{
    m_done = false;
}

void ContextOverride::Initialize()
{

}

void ContextOverride::Serialize( Json::Value &root ) const 
{
    Plugin::Serialize(root);
}

void ContextOverride::Deserialize( Json::Value &root )
{
    Plugin::Deserialize(root);
}

void ContextOverride::OnPreExecute( PreExecuteEvent &event, bool firstTime )
{
    if (!firstTime) return;
    if (m_done) return;
    u32 entry = LxEmulator.Proc()->GetEntryPoint();
    if (event.Cpu->EIP != entry) return;

    CONTEXT ctx;
    RefProcess *refproc = LxEmulator.RefProc();
    m_pi = refproc->GetProcessInformation();
    m_event = refproc->GetDebugEvent();

    byte origByte = refproc->SetInt3(entry);
    ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
    while (WaitForDebugEvent(m_event, INFINITE)) {
        if (m_event->dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
            m_event->u.Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT)
        {
            refproc->GetMainContext(&ctx, CONTEXT_CONTROL);
            ctx.Eip--;
            if (ctx.Eip != entry) {
                LxFatal("SyncDiff: Unexpected sync address %08x, should be %08x\n",
                    ctx.Eip, entry);
            }
            refproc->SetMainContext(&ctx);
            refproc->RestoreInt3(ctx.Eip, origByte);
            break;
        }
        ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
    }
    refproc->SetTF();

    LxInfo("Overriding Context..\n");
    OverrideContext(event.Cpu);
    m_done = true;
}

void ContextOverride::OverrideContext( Processor *cpu )
{
    CONTEXT ctx;
    RefProcess *refproc = LxEmulator.RefProc();
    refproc->GetMainContext(&ctx, CONTEXT_ALL);

    cpu->FPU()->Context()->ControlWord = (u16) ctx.FloatSave.ControlWord;

//     u32 refStackbase = refproc->GetMainStackBase();
//     u32 emuStackBase = cpu->Thr()->GetStack()->Top();
//     cpu->ESP = ctx.Esp;
//     cpu->EBP = ctx.Ebp;
//     pbyte stackptr = cpu->Mem->GetRawData(cpu->ESP);
//     B( ReadProcessMemory(m_pi->hProcess, (LPCVOID) ctx.Esp, stackptr, 
//         refStackbase - ctx.Esp, NULL) );

    cpu->EAX = ctx.Eax;
    cpu->EBX = ctx.Ebx;
    cpu->ECX = ctx.Ecx;
    cpu->EDX = ctx.Edx;
    cpu->ESI = ctx.Esi;
    cpu->EDI = ctx.Edi;
}
