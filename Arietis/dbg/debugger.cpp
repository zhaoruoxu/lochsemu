#include "stdafx.h"
#include "debugger.h"
#include "parallel.h"
#include "engine.h"
#include "processor.h"
#include "instruction.h"
#include "pemodule.h"
#include "breakpoint.h"
#include "event.h"

#include "gui/mainframe.h"

ADebugger::ADebugger(AEngine *engine) : m_engine(engine)
{
    m_archive = m_engine->GetArchive();
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
    m_crtEntryFound = false;
    m_mainEntry     = 0;
}

void ADebugger::OnPreExecute( PreExecuteEvent &event )
{
    m_currProcessor = event.Cpu;
    m_currInst      = event.Inst;

//     if (!m_crtEntryFound && m_archive->BreakOnCRTEntry) {
//         AnalyzeCRTEntry(cpu, inst);
//     }

    CheckBreakpoints(m_currProcessor, m_currInst);

    switch (m_state) {
    case STATE_SINGLESTEP:
        {
            DoPreExecSingleStep(m_currProcessor, m_currInst);
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
            if (m_currProcessor->EIP == m_stepOverEip) {
                m_state = STATE_SINGLESTEP;
                m_stepOverEip = 0;
                DoPreExecSingleStep(m_currProcessor, m_currInst);
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

void ADebugger::AddBreakpoint( u32 eip, const std::string &desc )
{
    uint module     = m_currProcessor->GetModule(eip);
    const ModuleInfo *minfo = m_currProcessor->Proc()->GetModuleInfo(module);
    Breakpoint bp(module, eip - minfo->ImageBase, desc, true);
    bp.Address      = eip;
    bp.ModuleName   = minfo->Name;

    {
        SyncObjectLock lock(*m_archive);
        m_breakpoints.push_back(bp);
    }
}

void ADebugger::ToggleBreakpoint(u32 eip)
{
    SyncObjectLock lock(*m_archive);

    for (auto &bp : m_breakpoints) {
        if (bp.Address == eip) {
            bp.Enabled = !bp.Enabled;       // toggle existing breakpoint
            return;
        }
    }

    AddBreakpoint(eip, "user");
}

void ADebugger::RemoveBreakpoint(u32 eip)
{
    SyncObjectLock lock(*m_archive);
    auto iter = m_breakpoints.begin();
    for (; iter != m_breakpoints.end(); iter++) {
        if (iter->Address == eip) {
            m_breakpoints.erase(iter);
            break;
        }
    }
}

const Breakpoint * ADebugger::GetBreakpoint( u32 eip ) const
{
    for (auto &bp : m_breakpoints) {
        if (bp.Address == eip) 
            return &bp;
    }
    return NULL;
}

void ADebugger::CheckBreakpoints( const Processor *cpu, const Instruction *inst )
{
    for (auto &bp : m_breakpoints) {
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

void ADebugger::OnProcessPreRun( ProcessPreRunEvent &event )
{
    


    if (g_config.GetInt("Debugger", "BreakOnMainModuleEntry", 1) != 0) {
        m_state = STATE_SINGLESTEP;
        LxInfo("Main module entry encountered\n");
    }

    //m_mainEntry = cpu->EIP;
}

void ADebugger::OnProcessPostLoad( ProcessPostLoadEvent &event )
{
    m_mainEntry = event.Loader->GetModuleInfo(0)->EntryPoint;

    // update all breakpoints' runtime info
    {
        //SyncObjectLock lock(*m_archive);
        for (auto &bp : m_breakpoints) {
            const ModuleInfo *minfo = event.Loader->GetModuleInfo(bp.Module);
            bp.ModuleName = minfo->Name;
            bp.Address = minfo->ImageBase + bp.Offset;
        }
    }
}

void ADebugger::OnTerminate()
{
    m_state = STATE_TERMINATED;
    m_semaphore.Post();
}

void ADebugger::UpdateInstContext( InstContext *ctx ) const
{
    for (int i = 0; i < InstContext::RegCount; i++) {
        ctx->regs[i]        = m_currProcessor->GP_Regs[i].X32;
    }
    ctx->Eip                = m_currProcessor->EIP;

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
    for (int i = 0; i < InstContext::RegCount; i++) {
        ctx->regs[i]        = m_currProcessor->GP_Regs[i].X32;
    }
    ctx->Eip                = eip;

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

// void ADebugger::AnalyzeCRTEntry( const Processor *cpu, const Instruction *inst )
// {
//     if (cpu->GetCurrentModule() != 0) return;
// 
//     static const int EipDistMax = 2048;
//     int dist = abs((int) (cpu->EIP - m_mainEntry));
//     if (dist > EipDistMax) return;
//     if (inst->Main.Inst.Opcode != 0xe8) return;
//     if (PAGE_LOW(inst->Main.Inst.AddrValue) != 0) return;
//     if (cpu->GetModule(inst->Main.Inst.AddrValue) != 0) return;
// 
// 
//     LxInfo("CRT entry found at %08x, entry = %08x\n", cpu->EIP, 
//         inst->Main.Inst.AddrValue);
//     m_crtEntryFound = true;
//     if (GetBreakpoint(cpu->EIP) == NULL)
//         AddBreakpoint(cpu->EIP, "crt_entry");
//     m_engine->SaveArchive();
// }

void ADebugger::Serialize( Json::Value &root ) const 
{
    Json::Value bps;
    for (const Breakpoint &bp : m_breakpoints) {
        Json::Value b;
        bp.Serialize(b);
        bps.append(b);
    }

    root["breakpoints"]     = bps;
}

void ADebugger::Deserialize( Json::Value &root )
{
    Json::Value bps = root["breakpoints"];
    if (!bps.empty()) {
        for (uint i = 0; i < bps.size(); i++) {
            Breakpoint bp;
            bp.Deserialize(bps[i]);
            m_breakpoints.push_back(bp);
        }
    }
}
