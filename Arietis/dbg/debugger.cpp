#include "stdafx.h"
#include "debugger.h"
#include "parallel.h"
#include "engine.h"
#include "processor.h"
#include "instruction.h"
#include "pemodule.h"

#include "gui/mainframe.h"

ADebugger::ADebugger(AEngine *engine) : m_engine(engine)
{
    m_archive = engine->GetArchive();
}

ADebugger::~ADebugger()
{
}

void ADebugger::Initialize()
{
    if (g_config.GetInt("Debugger", "BreakOnEntry", 1) != 0) {
        m_state     = STATE_SINGLESTEP;
    } else {
        m_state     = STATE_RUNNING;
    }
    m_stepOverEip   = 0;
    m_currInst      = NULL;
    m_currProcessor = NULL;
}

void ADebugger::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    m_currProcessor = cpu;
    m_currInst      = inst;
    CheckBreakpoints(cpu, inst);

    switch (m_state) {
    case STATE_SINGLESTEP:
        {
            DoPreExecSingleStep(cpu, inst);
        } break;
    case STATE_RUNNING:
        {
            // do nothing
        } break;
    case STATE_STEPOUT:
        {
            // not implemented
        } break;
    case STATE_STEPOVER:
        {
            if (cpu->EIP == m_stepOverEip) {
                m_state = STATE_SINGLESTEP;
                m_stepOverEip = 0;
                DoPreExecSingleStep(cpu, inst);
            }
        } break;
    case STATE_TERMINATED:
        {
            exit(0);
        } break;
    default:
        LxFatal("invalid Arietis.Debugger state\n");
    }
}

// void ADebugger::OnPostExecute( Processor *cpu, const Instruction *inst )
// {
//     if (m_state == STATE_SINGLESTEP) {
//         m_engine->GetGUI()->PostExecSingleStepCallback(cpu, inst);
//     }
// }

void ADebugger::OnStepInto()
{
    Assert(m_state == STATE_SINGLESTEP);
    m_semaphore.Post();
    m_engine->ReportBusy(true);
}

void ADebugger::OnStepOver()
{
    if (m_state == STATE_STEPOVER) return;

    if (NULL == m_currProcessor || NULL == m_currInst) return;

    
    if (Instruction::IsCall(m_currInst)) {
        m_state = STATE_STEPOVER;
        m_stepOverEip = m_currProcessor->EIP + m_currInst->Length;
    }

    m_semaphore.Post();
    m_engine->ReportBusy(true);
}

void ADebugger::OnRun()
{
    m_state = STATE_RUNNING;
    m_semaphore.Post();
    m_engine->ReportBusy(true);
}

void ADebugger::OnStepOut()
{
    //m_engine->GetGUI()->DebugLog("StepOut not implemented");
    LxError("StepOut not implemented\n");
}

void ADebugger::OnToggleBreakpoint(u32 eip)
{
    SyncObjectLock lock(*m_archive);

    for (auto &bp : m_archive->Breakpoints) {
        if (bp.Address == eip) {
            bp.Enabled = !bp.Enabled;       // toggle existing breakpoint
            //m_archive->Unlock();
            return;
        }
    }

    // add a new breakpoint
    uint module     = m_currProcessor->GetModule(eip);
    const ModuleInfo *minfo = m_currProcessor->Proc()->GetModuleInfo(module);
    Breakpoint bp(module, eip - minfo->ImageBase, "user", true);
    bp.Address      = eip;
    bp.ModuleName   = minfo->Name;
    m_archive->Breakpoints.push_back(bp);
}

void ADebugger::RemoveBreakpoint(u32 eip)
{
    SyncObjectLock lock(*m_archive);
    auto iter = m_archive->Breakpoints.begin();
    for (; iter != m_archive->Breakpoints.end(); iter++) {
        if (iter->Address == eip) {
            m_archive->Breakpoints.erase(iter);
            break;
        }
    }
}

const Breakpoint * ADebugger::GetBreakpoint( u32 eip ) const
{
    for (auto &bp : m_archive->Breakpoints) {
        if (bp.Address == eip) 
            return &bp;
    }
    return NULL;
}

void ADebugger::CheckBreakpoints( const Processor *cpu, const Instruction *inst )
{
    //SyncObjectLock lock(*m_archive);
    for (auto &bp : m_archive->Breakpoints) {
        if (cpu->EIP == bp.Address && bp.Enabled) {
            m_state = STATE_SINGLESTEP;
            break;
        }
    }
}

void ADebugger::DoPreExecSingleStep( const Processor *cpu, const Instruction *inst )
{
    m_engine->GetGUI()->PreExecSingleStepCallback(cpu, inst);
    m_engine->ReportBusy(false);
    m_semaphore.Wait();
}

void ADebugger::OnProcPreRun( const Process *proc, const Processor *cpu )
{
    // update all breakpoints' runtime info
    {
        //SyncObjectLock lock(*m_archive);
        for (auto &bp : m_archive->Breakpoints) {
            const ModuleInfo *minfo = proc->GetModuleInfo(bp.Module);
            bp.ModuleName = minfo->Name;
            bp.Address = minfo->ImageBase + bp.Offset;
        }
    }

    if (g_config.GetInt("Debugger", "BreakOnMainModuleEntry", 1) != 0) {
        m_state = STATE_SINGLESTEP;
        LxInfo("Main module entry encountered\n");
    }
}

void ADebugger::OnTerminate()
{
    m_state = STATE_TERMINATED;
    m_semaphore.Post();
}

void ADebugger::UpdateInstContext( InstContext *ctx ) const
{
    for (int i = InstContext::RegIndexGP; i < InstContext::RegIndexGP + InstContext::RegGPCount; i++) {
        ctx->regs[i]        = m_currProcessor->GP_Regs[i].X32;
    }
    ctx->regs[InstContext::RegIndexEip] = m_currProcessor->EIP;

    ctx->flags[InstContext::OF]         = m_currProcessor->OF;
    ctx->flags[InstContext::SF]         = m_currProcessor->SF;
    ctx->flags[InstContext::ZF]         = m_currProcessor->ZF;
    ctx->flags[InstContext::AF]         = m_currProcessor->AF;
    ctx->flags[InstContext::PF]         = m_currProcessor->PF;
    ctx->flags[InstContext::CF]         = m_currProcessor->CF;
//     ctx->flags[6]            = m_currProcessor->TF;
//     ctx->flags[7]            = m_currProcessor->IF;
//     ctx->flags[8]            = m_currProcessor->DF;
//     ctx->flags[9]            = m_currProcessor->NT;
//     ctx->flags[10]           = m_currProcessor->RF;

    uint module             = m_currProcessor->GetCurrentModule();
    const ModuleInfo *minfo = m_currProcessor->Proc()->GetModuleInfo(module);
    ctx->moduleName         = minfo->Name;
    ctx->moduleImageBase    = minfo->ImageBase;
}

void ADebugger::UpdateTraceContext( TraceContext *ctx, u32 eip ) const
{
    for (int i = InstContext::RegIndexGP; i < InstContext::RegIndexGP + InstContext::RegGPCount; i++) {
        ctx->regs[i]        = m_currProcessor->GP_Regs[i].X32;
    }
    ctx->regs[InstContext::RegIndexEip] = eip;

    ctx->flags[InstContext::OF]         = m_currProcessor->OF;
    ctx->flags[InstContext::SF]         = m_currProcessor->SF;
    ctx->flags[InstContext::ZF]         = m_currProcessor->ZF;
    ctx->flags[InstContext::AF]         = m_currProcessor->AF;
    ctx->flags[InstContext::PF]         = m_currProcessor->PF;
    ctx->flags[InstContext::CF]         = m_currProcessor->CF;

    uint module             = m_currProcessor->GetModule(eip);
    const ModuleInfo *minfo = m_currProcessor->Proc()->GetModuleInfo(module);
    ctx->moduleName         = minfo->Name;
    ctx->moduleImageBase    = minfo->ImageBase;
}
