#include "stdafx.h"
#include "diff.h"
#include "console.h"
#include "LochsDbg.h"
#include "process.h"
#include "emulator.h"
#include "processor.h"
#include "debugger.h"
#include "tracer.h"
#include "coprocessor.h"
#include "stack.h"
#include "refproc.h"


Diff LxDiff;


Diff::Diff() 
{
    m_refProcess    = NULL;
    m_enabled       = false;
    m_synced        = false;
    m_startAddress  = 0;
    m_lastEip       = 0;
    m_syncStart     = 0;
}

Diff::~Diff()
{

}

void Diff::Initialize( void )
{
    m_enabled               = g_config.GetInt("Diff", "Enabled", 0) != 0;
    m_synced                = false;
    m_enableTraceWhenSync   = g_config.GetInt("Diff", "EnableTraceWhenSync", 1) != 0;
    m_breakOnDiff           = g_config.GetInt("Diff", "BreakOnDiff", 1) != 0;
    m_breakOnSync           = g_config.GetInt("Diff", "BreakOnSync", 1) != 0;
    m_flagStepOut           = false;
    m_syncStart             = g_config.GetUint("Diff", "SyncStartAddress", 0);

    m_overrideFpuCtrl       = g_config.GetInt("Diff", "OverrideFPUControlWord", 1) != 0;
    m_overrideStack         = g_config.GetInt("Diff", "OverrideStackContent", 1) != 0;
    m_overrideCommonRegs    = g_config.GetInt("Diff", "OverrideCommonRegisters", 1) != 0;

    m_diffCommonRegs        = g_config.GetInt("Diff", "DiffCommonRegisters", 0) != 0;
    m_diffEip               = g_config.GetInt("Diff", "DiffEip", 1) != 0;
    m_diffStackRegs         = g_config.GetInt("Diff", "DiffStackRegisters", 1) != 0;
}

void Diff::OnProcessPreRun( const Process *proc, Processor *cpu )
{
    m_startAddress = proc->GetEntryPoint();
}

void Diff::OnProcessorPreExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enabled) return;

    CONTEXT ctx;
    if (m_syncStart == 0) m_syncStart = m_startAddress;
    if (!m_synced && cpu->EIP == m_syncStart) {
        m_synced = true;
        if (m_breakOnSync) {
            LxDebugger.BreakOnNextInst("Sync started");
        }
        // Reference process runs to RefStartAddress
        byte origStartByte = m_refProcess->SetInt3(m_syncStart);
        ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        while (WaitForDebugEvent(m_event, INFINITE)) {
            if (m_event->dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
                m_event->u.Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT) {
                    m_refProcess->GetMainContext(&ctx, CONTEXT_CONTROL);
                    ctx.Eip--;
                    if (ctx.Eip != m_syncStart) {
                        LxFatal("LochsDiff: Unexpected sync address %08x, should be %08x\n",
                            ctx.Eip, m_syncStart);
                    }
                    LxInfo("LochsDiff: Sync entry point %08x\n", ctx.Eip);
                    m_refProcess->SetMainContext(&ctx);
                    m_refProcess->RestoreInt3(ctx.Eip, origStartByte);
                    break;
            }
            ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        }
        // Enter single step mode
        m_refProcess->SetTF();
        // Enable trace
        if (m_enableTraceWhenSync) LxDebugger.GetTracer()->Enable(true);

        // Override thread context
        OverrideContext(cpu);
    }
}

void Diff::OnProcessorPostExecute( Processor *cpu, const Instruction *inst )
{
    CONTEXT ctx;
    if (!m_enabled || !m_synced) return;

    bool retReached = 
        inst->Main.Inst.Opcode == 0xC3 /* RET near */ ||
        inst->Main.Inst.Opcode == 0xCB /* RET far */ ||
        inst->Main.Inst.Opcode == 0xC2 /* RET imm16 near */ ||
        inst->Main.Inst.Opcode == 0xCA /* RET imm16 far */;

    // If last executed instruction is a WinAPI call, we have to skip 
    // this procedure call in the reference process. To do this, keep running the 
    // reference process until cpu.EIP is reached.
    if (cpu->HasExecFlag(LX_EXEC_WINAPI_CALL) || 
        cpu->HasExecFlag(LX_EXEC_WINAPI_JMP) || 
        cpu->HasExecFlag(LX_EXEC_PREFIX_REP) ||
        cpu->HasExecFlag(LX_EXEC_PREFIX_REPNE) ||
        (m_flagStepOut && retReached) ) 
    {
        if (m_flagStepOut) {
            LxInfo("Stepping Out...\n");
            m_flagStepOut = false;
            LxDebugger.PrintContext();
            LxDebugger.BreakOnNextInst("Stepped out");
        }
        u8 origByte = m_refProcess->SetInt3(cpu->EIP);
        ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        while (WaitForDebugEvent(m_event, INFINITE)) {
            if (m_event->dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
                m_event->u.Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT) 
            {
                m_refProcess->GetMainContext(&ctx, CONTEXT_CONTROL);
                ctx.Eip--;
                if (ctx.Eip != cpu->EIP) {
                    LxFatal("LochsDiff: Unexpected EIP value after skipping WinAPI call %08x,"
                        " should be %08x\n", ctx.Eip, cpu->EIP);
                }
                m_refProcess->SetMainContext(&ctx);
                m_refProcess->RestoreInt3(ctx.Eip, origByte);
                break;
            } else if (m_event->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
                m_enabled = false;
                m_synced = false;
                break;
            }
            ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        }
    } else if (m_flagStepOut) {
        return;
    } else {
        ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        while (WaitForDebugEvent(m_event, INFINITE)) {
            if (m_event->u.Exception.ExceptionRecord.ExceptionCode == STATUS_SINGLE_STEP) {
                break;
            } else if (m_event->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
                m_enabled = m_synced = false;
                break;
            }
            ContinueDebugEvent(m_pi->dwProcessId, m_pi->dwThreadId, DBG_CONTINUE);
        }
    }

    if (!m_synced || !m_enabled) return;
    // enter single-step mode
    m_refProcess->SetTF();

    // compare runtime context
    m_refProcess->GetMainContext(&ctx, CONTEXT_ALL);
    if ( !CompareRuntimeContext(cpu, &ctx) && m_breakOnDiff) {
        LxDebugger.PrintContext();
        PrintRefRegisters();
        LxDebugger.BreakOnNextInst("Runtime context mismatch");
    }
}

void Diff::OnProcessPreLoad( PeLoader *loader )
{
    m_refProcess    = loader->Emu()->RefProc();
    m_pi            = m_refProcess->GetProcessInformation();
    m_event         = m_refProcess->GetDebugEvent();
}

bool Diff::CompareRuntimeContext( const Processor *cpu, const CONTEXT *ctx )
{
#define DIFF(...) do { StdError(__VA_ARGS__); tracer->Trace(__VA_ARGS__); okay = false; } while(0)

    Tracer *tracer = LxDebugger.GetTracer();
    bool okay = true;
    if (m_diffEip && cpu->EIP != ctx->Eip) {
        DIFF("<<DIFF>> Emu_Eip=%08x, Ref_Eip=%08x\n", cpu->EIP, ctx->Eip);
    }

    if (m_diffStackRegs) {
        if (cpu->ESP != ctx->Esp)
            DIFF("<<DIFF>> Emu_Esp=%08x, Ref_Esp=%08x\n", cpu->ESP, ctx->Esp);
        if (cpu->EBP != ctx->Ebp)
            DIFF("<<DIFF>> Emu_Ebp=%08x, Ref_Ebp=%08x\n", cpu->EBP, ctx->Ebp);
    }

    if (m_diffCommonRegs) {
        if (cpu->EAX != ctx->Eax)
            DIFF("<<DIFF>> Emu_Eax=%08x, Ref_Eax=%08x\n", cpu->EAX, ctx->Eax);
        if (cpu->ECX != ctx->Ecx)
            DIFF("<<DIFF>> Emu_Ecx=%08x, Ref_Ecx=%08x\n", cpu->ECX, ctx->Ecx);
        if (cpu->EDX != ctx->Edx)
            DIFF("<<DIFF>> Emu_Edx=%08x, Ref_Edx=%08x\n", cpu->EDX, ctx->Edx);
        if (cpu->EBX != ctx->Ebx)
            DIFF("<<DIFF>> Emu_Ebx=%08x, Ref_Ebx=%08x\n", cpu->EBX, ctx->Ebx);
        if (cpu->ESI != ctx->Esi)
            DIFF("<<DIFF>> Emu_Esi=%08x, Ref_Esi=%08x\n", cpu->ESI, ctx->Esi);
        if (cpu->EDI != ctx->Edi)
            DIFF("<<DIFF>> Emu_Edi=%08x, Ref_Edi=%08x\n", cpu->EDI, ctx->Edi);
    }

    return okay;
#undef DIFF
}

void Diff::PrintRefRegisters( void )
{
    CONTEXT ctx;
    m_refProcess->GetMainContext(&ctx, CONTEXT_ALL);
    StdDumpDark("    eax[%08x] ecx[%08x] edx[%08x] ebx[%08x]\n    esp[%08x] ebp[%08x] esi[%08x] edi[%08x]\n",
        ctx.Eax, ctx.Ecx, ctx.Edx, ctx.Ebx, ctx.Esp, ctx.Ebp, ctx.Esi, ctx.Edi);
}

void Diff::OverrideContext(Processor *cpu)
{
    // Get initial reference context
    CONTEXT ctx;
    m_refProcess->GetMainContext(&ctx, CONTEXT_ALL);

    // Override FPU ctrl word
    if (m_overrideFpuCtrl) {
        LxInfo("Overriding FPU control word\n");
        cpu->FPU()->Context()->ControlWord = (u16) ctx.FloatSave.ControlWord;
    }

    // Override stack
    if (m_overrideStack) {
        LxInfo("Overriding stack content\n");
        u32 refStackbase = m_refProcess->GetMainStackBase();
        u32 emuStackBase = cpu->Thr()->GetStack()->Top();
        cpu->ESP = ctx.Esp;
        cpu->EBP = ctx.Ebp;
        pbyte stackptr = cpu->Mem->GetRawData(cpu->ESP);
        B( ReadProcessMemory(m_pi->hProcess, (LPCVOID) ctx.Esp, stackptr, 
            refStackbase - ctx.Esp, NULL) );
    }

    // Override common registers
    if (m_overrideCommonRegs) {
        LxInfo("Overriding common registers\n");
        cpu->EAX = ctx.Eax;
        cpu->EBX = ctx.Ebx;
        cpu->ECX = ctx.Ecx;
        cpu->EDX = ctx.Edx;
        cpu->ESI = ctx.Esi;
        cpu->EDI = ctx.Edi;
    }
}
