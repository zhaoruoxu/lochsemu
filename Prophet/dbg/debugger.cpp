#include "stdafx.h"
#include "debugger.h"
#include "parallel.h"
#include "engine.h"
#include "processor.h"
#include "instruction.h"
#include "pemodule.h"
#include "breakpoint.h"
#include "event.h"
#include "process.h"
#include "emulator.h"

#include "gui/mainframe.h"

ProDebugger::ProDebugger(ProEngine *engine) : m_engine(engine)
{
    m_archive = m_engine->GetArchive();
    m_currTid = 0;  // Main thread
    m_switchThreadOnBreak = true;

    ZeroMemory(m_threads, sizeof(m_threads));

}

ProDebugger::~ProDebugger()
{
    for (int i = 0; i < MaxThreads; i++) {
        SAFE_DELETE(m_semaphore[i]);
    }
}

void ProDebugger::Initialize()
{
    for (int i = 0; i < MaxThreads; i++) {
        m_state[i]          = STATE_RUNNING;
        m_stepOverEip[i]    = 0;
        m_currInst[i]       = NULL;
        m_cpu[i]  = NULL;
        m_threads[i]        = NULL;
        m_semaphore[i]      = NULL;
    }

}

void ProDebugger::OnPreExecute( PreExecuteEvent &event )
{
    //if (event.Cpu->IntID != 0) return;  // ignore other threads

    const int Tid = event.Cpu->IntID;

    Assert(m_cpu[Tid] == event.Cpu);
    m_currInst[Tid]      = event.Inst;

    if (m_state[Tid] == STATE_RUNNING_NOBP) {
        m_mrs[Tid].clear();
        m_mws[Tid].clear();
        return; // ignore everything
    }

    //m_engine->GetGUI()->OnPreExecute(event);
    CheckBreakpoints(m_cpu[Tid], m_currInst[Tid]);

    switch (m_state[event.Cpu->IntID]) {
    case STATE_SINGLESTEP:
        {
            DoPreExecSingleStep(m_cpu[Tid], m_currInst[Tid]);
        } break;
    case STATE_RUNNING:
        {
            
        } break;
    case STATE_STEPOUT:
        {
            if (m_cpu[Tid]->GetCurrentModule() == 0) {
                m_state[Tid] = STATE_SINGLESTEP;
                DoPreExecSingleStep(m_cpu[Tid], m_currInst[Tid]);
            }
        } break;
    case STATE_STEPOVER:
        {
            if (m_cpu[Tid]->EIP == m_stepOverEip[Tid]) {
                m_state[Tid] = STATE_SINGLESTEP;
                m_stepOverEip[Tid] = 0;
                DoPreExecSingleStep(m_cpu[Tid], m_currInst[Tid]);
            }
        } break;
    case STATE_TERMINATED:
        {
            exit(0);
        } break;
    default:
        LxFatal("invalid Prophet Debugger state\n");
    }

    m_mrs[Tid].clear();
    m_mws[Tid].clear();
}

void ProDebugger::OnStepInto()
{
    Assert(m_state[m_currTid] == STATE_SINGLESTEP);
    Assert(m_semaphore[m_currTid]);
    m_semaphore[m_currTid]->Post();
    
    m_engine->ReportBusy(true);
}

void ProDebugger::OnStepOver()
{
    if (m_state[m_currTid] == STATE_STEPOVER) return;
    if (NULL == m_cpu[m_currTid] || NULL == m_currInst[m_currTid]) 
        return;

    if (Instruction::IsCall(m_currInst[m_currTid])) {
        m_state[m_currTid] = STATE_STEPOVER;
        m_stepOverEip[m_currTid] = m_cpu[m_currTid]->EIP + m_currInst[m_currTid]->Length;
    }

    m_semaphore[m_currTid]->Post();
    m_engine->ReportBusy(true);
}

void ProDebugger::OnRun()
{
    m_state[m_currTid] = STATE_RUNNING;
    Assert(m_semaphore[m_currTid]);
    m_semaphore[m_currTid]->Post();
    m_engine->ReportBusy(true);
}

void ProDebugger::OnRunNoBp()
{
    m_state[m_currTid] = STATE_RUNNING_NOBP;
    Assert(m_semaphore[m_currTid]);
    m_semaphore[m_currTid]->Post();
    m_engine->ReportBusy(true);
}

void ProDebugger::OnStepOut()
{
    m_state[m_currTid] = STATE_STEPOUT;
    Assert(m_semaphore[m_currTid]);
    m_semaphore[m_currTid]->Post();
    m_engine->ReportBusy(true);
}

void ProDebugger::AddBreakpoint( u32 eip, const std::string &desc )
{
    uint module     = LxEmulator.Mem()->GetSection(eip)->Module(); //m_currProcessor[m_currTid]->GetModule(eip);
    const ModuleInfo *minfo = LxEmulator.Proc()->GetModuleInfo(module);
    Breakpoint bp(module, eip - minfo->ImageBase, desc, true);
    bp.Address      = eip;
    bp.ModuleName   = minfo->Name;

    {
        SyncObjectLock lock(*m_archive);
        m_breakpoints.push_back(bp);
    }
}

void ProDebugger::ToggleBreakpoint(u32 eip)
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

void ProDebugger::RemoveBreakpoint(u32 eip)
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

const Breakpoint * ProDebugger::GetBreakpoint( u32 eip ) const
{
    for (auto &bp : m_breakpoints) {
        if (bp.Address == eip) 
            return &bp;
    }
    return NULL;
}

void ProDebugger::CheckBreakpoints( const Processor *cpu, const Instruction *inst )
{
    for (auto &bp : m_breakpoints) {
        if (cpu->EIP == bp.Address && bp.Enabled) {
            m_state[cpu->IntID] = STATE_SINGLESTEP;
            if (m_switchThreadOnBreak) {
                SetCurrentThread(cpu->IntID);
            }
            break;
        }
    }
}

void ProDebugger::DoPreExecSingleStep( const Processor *cpu, const Instruction *inst )
{
    if (cpu->IntID == m_currTid) {
        m_engine->ReportBusy(false);
        m_engine->GetGUI()->OnPreExecSingleStep(cpu);
    }
    m_engine->GetGUI()->OnThreadStateChange(m_threads[cpu->IntID]);
    m_semaphore[cpu->IntID]->Wait();
}

void ProDebugger::OnProcessPreRun( ProcessPreRunEvent &event )
{
    m_engine->ReportBusy(true);
}

void ProDebugger::OnProcessPostLoad( ProcessPostLoadEvent &event )
{
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

void ProDebugger::OnTerminate()
{
    for (int i = 0; i < MaxThreads; i++) {
        m_state[i] = STATE_TERMINATED;
        if (m_semaphore[i])
            m_semaphore[i]->Post();
    }
}

void ProDebugger::UpdateInstContext( const Processor *cpu, InstContext *ctx ) const
{
    Assert(m_cpu[cpu->IntID] != NULL);
    for (int i = 0; i < InstContext::RegCount; i++) {
        ctx->Regs[i]        = m_cpu[cpu->IntID]->GP_Regs[i].X32;
    }
    ctx->Eip                = m_cpu[cpu->IntID]->EIP;

    if (Instruction::IsConditionalJump(ctx->Inst)) {
        ctx->JumpTaken      = m_cpu[cpu->IntID]->IsJumpTaken(ctx->Inst);
    }

    ctx->Flags[InstContext::OF]         = m_cpu[cpu->IntID]->OF;
    ctx->Flags[InstContext::SF]         = m_cpu[cpu->IntID]->SF;
    ctx->Flags[InstContext::ZF]         = m_cpu[cpu->IntID]->ZF;
    ctx->Flags[InstContext::AF]         = m_cpu[cpu->IntID]->AF;
    ctx->Flags[InstContext::PF]         = m_cpu[cpu->IntID]->PF;
    ctx->Flags[InstContext::CF]         = m_cpu[cpu->IntID]->CF;
//     ctx->flags[6]            = m_currProcessor->TF;
//     ctx->flags[7]            = m_currProcessor->IF;
//     ctx->flags[8]            = m_currProcessor->DF;
//     ctx->flags[9]            = m_currProcessor->NT;
//     ctx->flags[10]           = m_currProcessor->RF;

    uint module             = m_cpu[cpu->IntID]->GetCurrentModule();
    const ModuleInfo *minfo = m_cpu[cpu->IntID]->Proc()->GetModuleInfo(module);
    ctx->ModuleName         = minfo->Name;
    ctx->ModuleImageBase    = minfo->ImageBase;

    ctx->MRs = m_mrs[cpu->IntID];
    ctx->MWs = m_mws[cpu->IntID];

    ctx->Tid = cpu->IntID;
    ctx->ExternalTid = cpu->Thr()->ExtID;
}

void ProDebugger::UpdateTraceContext( const Processor *cpu, TraceContext *ctx, u32 eip ) const
{
    Assert(m_cpu[cpu->IntID] != NULL);
    for (int i = 0; i < InstContext::RegCount; i++) {
        ctx->Regs[i]        = m_cpu[cpu->IntID]->GP_Regs[i].X32;
    }
    ctx->Eip                = eip;

    ctx->Flags[InstContext::OF]         = m_cpu[cpu->IntID]->OF;
    ctx->Flags[InstContext::SF]         = m_cpu[cpu->IntID]->SF;
    ctx->Flags[InstContext::ZF]         = m_cpu[cpu->IntID]->ZF;
    ctx->Flags[InstContext::AF]         = m_cpu[cpu->IntID]->AF;
    ctx->Flags[InstContext::PF]         = m_cpu[cpu->IntID]->PF;
    ctx->Flags[InstContext::CF]         = m_cpu[cpu->IntID]->CF;

    uint module             = m_cpu[cpu->IntID]->GetModule(eip);
    const ModuleInfo *minfo = m_cpu[cpu->IntID]->Proc()->GetModuleInfo(module);
    ctx->ModuleName         = minfo->Name;
    ctx->ModuleImageBase    = minfo->ImageBase;

    ctx->MRs = m_mrs[cpu->IntID];
    ctx->MWs = m_mws[cpu->IntID];

    ctx->Tid = cpu->IntID;
    ctx->ExternalTid = cpu->Thr()->ExtID;
}

void ProDebugger::Serialize( Json::Value &root ) const 
{
    Json::Value bps;
    for (const Breakpoint &bp : m_breakpoints) {
        Json::Value b;
        bp.Serialize(b);
        bps.append(b);
    }

    root["breakpoints"]     = bps;
    root["switch_thread_on_break"] = m_switchThreadOnBreak;
}

void ProDebugger::Deserialize( Json::Value &root )
{
    Json::Value bps = root["breakpoints"];
    if (!bps.empty()) {
        for (uint i = 0; i < bps.size(); i++) {
            Breakpoint bp;
            bp.Deserialize(bps[i]);
            m_breakpoints.push_back(bp);
        }
    }

    m_switchThreadOnBreak = root.get("switch_thread_on_break", 
        m_switchThreadOnBreak).asBool();
}

void ProDebugger::SetState( int tid, State s )
{
    if (m_state[tid] == STATE_RUNNING_NOBP) return;
    m_state[tid] = s;
}

void ProDebugger::OnMemRead( MemReadEvent &event )
{
    if (event.NBytes == 4) {
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr, 4, *((u32p) event.Data));
    } else if (event.NBytes == 1) {
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr, 1, *((u8p)  event.Data));
    } else if (event.NBytes == 2) {
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr, 2, *((u16p) event.Data));
    } else if (event.NBytes == 8) {
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr, 4, *((u32p) event.Data));
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr+4, 4, *((u32p) (event.Data+4)));
    } else if (event.NBytes == 16) {
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr, 4, *((u32p) event.Data));
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr+4, 4, *((u32p) (event.Data+4)));
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr+8, 4, *((u32p) (event.Data+8)));
        m_mrs[event.Cpu->IntID].emplace_back(event.Addr+12, 4, *((u32p) (event.Data+12)));
    } else {
        LxFatal("shit happens\n");
    }
}

void ProDebugger::OnMemWrite( MemWriteEvent &event )
{
    if (event.NBytes == 4) {
        m_mws[event.Cpu->IntID].emplace_back(event.Addr, 4, *((u32p) event.Data));
    } else if (event.NBytes == 1) {
        m_mws[event.Cpu->IntID].emplace_back(event.Addr, 1, *((u8p)  event.Data));
    } else if (event.NBytes == 2) {
        m_mws[event.Cpu->IntID].emplace_back(event.Addr, 2, *((u16p) event.Data));
    } else if (event.NBytes == 8) {
        m_mws[event.Cpu->IntID].emplace_back(event.Addr, 4, *((u32p) event.Data));
        m_mws[event.Cpu->IntID].emplace_back(event.Addr+4, 4, *((u32p) (event.Data+4)));
    } else if (event.NBytes == 16) {
        m_mws[event.Cpu->IntID].emplace_back(event.Addr, 4, *((u32p) event.Data));
        m_mws[event.Cpu->IntID].emplace_back(event.Addr+4, 4, *((u32p) (event.Data+4)));
        m_mws[event.Cpu->IntID].emplace_back(event.Addr+8, 4, *((u32p) (event.Data+8)));
        m_mws[event.Cpu->IntID].emplace_back(event.Addr+12, 4, *((u32p) (event.Data+12)));
    } else {
        LxFatal("shit happens\n");
    }
}

void ProDebugger::OnThreadCreate( ThreadCreateEvent &event )
{
    //LxInfo("Thread created: %d %08x\n", event.Thrd->IntID, event.Thrd->ExtID);
    const int tid = event.Thrd->IntID;
    Assert(m_threads[tid] == NULL);
    m_threads[tid]   = event.Thrd;
    m_cpu[tid]       = event.Thrd->CPU();
    m_state[tid]         = STATE_RUNNING;
    m_stepOverEip[tid]   = 0;
    Assert(m_semaphore[tid] == NULL);
    m_semaphore[tid] = new Semaphore();
//     m_mrs[id].clear();
//     m_mws[id].clear();
}

void ProDebugger::OnThreadExit( ThreadExitEvent &event )
{
    //LxInfo("Thread exited: %d %08x\n", event.Thrd->IntID, event.Thrd->ExtID);

    const int tid = event.Thrd->IntID;

    if (tid == m_currTid) {
        SetCurrentThread(0);    // main thread
    }

    Assert(m_threads[tid] != NULL);
    m_threads[tid] = NULL;
    Assert(m_semaphore[tid] != NULL);
    m_semaphore[tid]->Post();
    SAFE_DELETE(m_semaphore[tid]);
    m_mrs[tid].clear();
    m_mws[tid].clear();
}

void ProDebugger::SetCurrentThread( int tid )
{
    Assert(m_threads[tid] != NULL);
    m_currTid = tid;
    //bool isSingleStep = m_state[tid] == STATE_SINGLESTEP;
    m_state[tid] = STATE_SINGLESTEP;

    LxInfo("--- DEBUGGER: Current Thread is %d ---\n", tid);

    m_engine->ReportBusy(false);
    //if (isSingleStep) {
        m_engine->GetGUI()->OnPreExecSingleStep(m_cpu[tid]);
    //}
//     else {
//         m_engine->GetGUI()->OnRefresh();
//     }
    //
}

Thread * ProDebugger::GetCurrentThread()
{
    Assert(m_threads[m_currTid]); 
    return m_threads[m_currTid];
}

const Thread * ProDebugger::GetCurrentThread() const
{
    Assert(m_threads[m_currTid]); 
    return m_threads[m_currTid];
}

ProDebugger::State ProDebugger::GetState( int tid ) const
{
    Assert(m_threads[tid] != NULL); 
    return m_state[tid];
}

Thread * ProDebugger::GetThread( int tid )
{
    return m_threads[tid];
}

const Thread * ProDebugger::GetThread( int tid ) const
{
    return m_threads[tid];
}

const char * ProDebugger::StateText[] = {
    "Running",
    "Running(NoBreak)",
    "SingleStep",
    "StepOver",
    "StepOut",
    "Terminated",
};

int ProDebugger::GetThreadCount() const
{
    int count = 0; 
    for (int i = 0; i < MaxThreads; i++)
        if (m_threads[i] != NULL) count++;
    return count;
}

