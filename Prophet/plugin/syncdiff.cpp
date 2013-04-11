#include "stdafx.h"
#include "syncdiff.h"
#include "event.h"
#include "peloader.h"
#include "refproc.h"
#include "dbg/debugger.h"
#include "processor.h"
#include "process.h"
#include "stack.h"
#include "coprocessor.h"
#include "emulator.h"

SyncDiff::SyncDiff( ProPluginManager *manager )
    : Plugin(manager, false, "SyncDiff", PreExecuteHandler | PostExecuteHandler | 
    ProcessPreRunHandler | ProcessPostLoadHandler)
{
    m_refProc   = NULL;
    m_pi        = NULL;
    m_event     = NULL;
    m_synced    = false;
}

void SyncDiff::Initialize()
{
    m_debugger = GetEngine()->GetDebugger();
}

void SyncDiff::OnPreExecute( PreExecuteEvent &event, bool firstTime )
{
    if (!firstTime) return;
    CONTEXT ctx;
    if (m_synced) return;
    if (event.Cpu->EIP != m_startAddr) return;

    // start sync
    m_synced = true;
    GetEngine()->BreakOnNextInst("Sync started");

    byte origByte = m_refProc->SetInt3(m_startAddr);
    ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
    while (WaitForDebugEvent(m_event, INFINITE)) {
        if (m_event->dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
            m_event->u.Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT)
        {
            m_refProc->GetMainContext(&ctx, CONTEXT_CONTROL);
            ctx.Eip--;
            if (ctx.Eip != m_startAddr) {
                LxFatal("SyncDiff: Unexpected sync address %08x, should be %08x\n",
                    ctx.Eip, m_startAddr);
            }
            m_refProc->SetMainContext(&ctx);
            m_refProc->RestoreInt3(ctx.Eip, origByte);
            break;
        }
        ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
    }

    m_refProc->SetTF();

    OverrideContext(event.Cpu);
}

void SyncDiff::OnPostExecute( PostExecuteEvent &event, bool firstTime )
{
    if (firstTime) return;

    if (event.Cpu->GetCurrentModule() != 0) return;

    if (!m_synced) return;
    const u32 opcode = event.Inst->Main.Inst.Opcode;
    bool retReached = opcode == 0xC3 || opcode == 0xcb || opcode == 0xc2 || opcode == 0xca;

    CONTEXT ctx;
    if (true || event.Cpu->HasExecFlag(LX_EXEC_WINAPI_CALL) ||
        event.Cpu->HasExecFlag(LX_EXEC_WINAPI_JMP) ||
        event.Cpu->HasExecFlag(LX_EXEC_PREFIX_REP) ||
        event.Cpu->HasExecFlag(LX_EXEC_PREFIX_REPNE) )
    {
        byte origByte = m_refProc->SetInt3(event.Cpu->EIP);
        ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        while (WaitForDebugEvent(m_event, INFINITE)) {
            if (m_event->dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
                m_event->u.Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT)
            {
                m_refProc->GetMainContext(&ctx, CONTEXT_CONTROL);
                ctx.Eip--;
                if (ctx.Eip != event.Cpu->EIP) {
                    LxFatal("SyncDiff: Unexpected EIP value after skipping: %08x, should be %08x\n",
                        ctx.Eip, event.Cpu->EIP);
                }
                m_refProc->SetMainContext(&ctx);
                m_refProc->RestoreInt3(ctx.Eip, origByte);
                break;
            } else if (m_event->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
                m_synced = false;
                Enable(false);
                break;
            }
            ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        }
    } else {
        ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        while (WaitForDebugEvent(m_event, INFINITE)) {
            if (m_event->u.Exception.ExceptionRecord.ExceptionCode == STATUS_SINGLE_STEP) {
                break;
            } else if (m_event->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
                m_synced = false;
                Enable(false);
                break;
            }
            ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        }
    }

    if (!m_synced || !IsEnabled()) return;
    //m_refProc->SetTF();

    m_refProc->GetMainContext(&ctx, CONTEXT_ALL);
    if (!CompareContext(event.Cpu, &ctx)) {
        LxError("Emu eip = %08x, Ref eip = %08x\n", event.Cpu->EIP, ctx.Eip);
        LxError("Emu esp = %08x, Ref esp = %08x\n", event.Cpu->ESP, ctx.Esp);
        LxError("Emu ebp = %08x, Ref ebp = %08x\n", event.Cpu->EBP, ctx.Ebp);
        LxError("Emu ecx = %08x, Ref ecx = %08x\n", event.Cpu->ECX, ctx.Ecx);
        GetEngine()->BreakOnNextInst("context diff");
    }
}

bool SyncDiff::CompareContext( const Processor *cpu, const CONTEXT *ctx )
{
    if (cpu->EIP != ctx->Eip) return false;
    if (cpu->ESP != ctx->Esp) return false;
    //if (cpu->ECX != ctx->Ecx) return false;
    //if (cpu->EBP != ctx->Ebp) return false;
    return true;
}

void SyncDiff::OnProcessPreRun( ProcessPreRunEvent &event, bool firstTime )
{
    if (!firstTime) return;
    m_startAddr = event.Proc->GetEntryPoint();
}

void SyncDiff::OnProcessPostLoad( ProcessPostLoadEvent &event, bool firstTime )
{
    if (firstTime) return;
    m_refProc   = event.Loader->Emu()->RefProc();
    m_pi        = m_refProc->GetProcessInformation();
    m_event     = m_refProc->GetDebugEvent();
}

void SyncDiff::OverrideContext( Processor *cpu )
{
    CONTEXT ctx;
    m_refProc->GetMainContext(&ctx, CONTEXT_ALL);

    cpu->FPU()->Context()->ControlWord = (u16) ctx.FloatSave.ControlWord;

    u32 refStackbase = m_refProc->GetMainStackBase();
    u32 emuStackBase = cpu->Thr()->GetStack()->Top();
    cpu->ESP = ctx.Esp;
    cpu->EBP = ctx.Ebp;
    pbyte stackptr = cpu->Mem->GetRawData(cpu->ESP);
    B( ReadProcessMemory(m_pi->hProcess, (LPCVOID) ctx.Esp, stackptr, 
        refStackbase - ctx.Esp, NULL) );

    cpu->EAX = ctx.Eax;
    cpu->EBX = ctx.Ebx;
    cpu->ECX = ctx.Ecx;
    cpu->EDX = ctx.Edx;
    cpu->ESI = ctx.Esi;
    cpu->EDI = ctx.Edi;
}

void SyncDiff::Serialize( Json::Value &root ) const 
{
    Plugin::Serialize(root);
}

void SyncDiff::Deserialize( Json::Value &root )
{
    Plugin::Deserialize(root);
}


