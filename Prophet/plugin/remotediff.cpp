#include "stdafx.h"
#include "remotediff.h"
#include "dbg/debugger.h"
#include "remotediff/pipeserver.h"

RemoteDiff::RemoteDiff( ProPluginManager *manager )
    : Plugin(manager, false, "RemoteDiff")
{

}

void RemoteDiff::Initialize()
{
    m_debugger = GetEngine()->GetDebugger();
    m_synced = false;
}

void RemoteDiff::OnPostExecute( PostExecuteEvent &event, bool firstTime )
{
    if (!firstTime) return;
    if (!m_synced) return;

    if (event.Cpu->GetCurrentModule() != 0) return;

    bool multiInsts = event.Cpu->HasExecFlag(LX_EXEC_WINAPI_CALL) ||
        event.Cpu->HasExecFlag(LX_EXEC_WINAPI_JMP) ||
        event.Cpu->HasExecFlag(LX_EXEC_PREFIX_REP) ||
        event.Cpu->HasExecFlag(LX_EXEC_PREFIX_REPNE);
    //SingleStepData data(event.Cpu->Thr()->IntID, event.Cpu->EIP, multiInsts);
    SyncData data(SE_SingleStep);
    data.SingleStep.ThreadId = event.Cpu->Thr()->IntID;
    data.SingleStep.Eip = event.Cpu->EIP;
    data.SingleStep.MultiInsts = multiInsts;


    LxInfo("Sending tid=%x eip=%08x\n", data.SingleStep.ThreadId, data.SingleStep.Eip);

    m_server.WriteData(data);

    SyncData res = m_server.ReadData<SyncData>();
    if (!CompareContextData(event, res.Context)) {
        LxError("Emu eip = %08x, Ref eip = %08x\n", event.Cpu->EIP, res.Context.Eip);
        LxError("Emu esp = %08x, Ref esp = %08x\n", event.Cpu->ESP, res.Context.Esp);
        GetEngine()->BreakOnNextInst("context diff");
    } else {
        LxInfo("Confirmed tid=%x, eip=%08x\n", res.Context.Tid, res.Context.Eip);
    }
}

void RemoteDiff::OnProcessPreRun( ProcessPreRunEvent &event, bool firstTime )
{
    if (!firstTime) return;

    if (!m_server.Connect("\\\\.\\pipe\\lochsemu")) {
        LxFatal("pipe connection failed");
    }

    const ModuleInfo *mi = event.Proc->GetModuleInfo(0);
    const Thread *thd = event.Proc->GetThread(0);
    //ProcessCreateData data(mi->ImageBase, mi->EntryPoint, thd->IntID, thd->CPU()->ESP);
    SyncData data(SE_ProcessCreate);
    data.ProcessCreate.ImageBase    = mi->ImageBase;
    data.ProcessCreate.StartAddress = mi->EntryPoint;
    data.ProcessCreate.ThreadId     = thd->IntID;
    data.ProcessCreate.Esp          = thd->CPU()->ESP;
    
    m_server.WriteData(data);
    m_synced = true;
}

bool RemoteDiff::CompareContextData( PostExecuteEvent &event, ContextData &data )
{
    if (event.Cpu->EIP != data.Eip) return false;
    //if (event.Cpu->ESP != data.Esp) return false;

    return true;
}

void RemoteDiff::OnThreadCreate( ThreadCreateEvent &event, bool firstTime )
{
    if (!firstTime) return;

    SyncData data(SE_ThreadCreate);
    data.ThreadCreate.ParentTid = event.Thrd->ParentId;
    data.ThreadCreate.Tid = event.Thrd->IntID;
    data.ThreadCreate.StartAddress = event.Thrd->GetThreadInfo()->EntryPoint;
    data.ThreadCreate.Esp = event.Thrd->CPU()->ESP;

    m_server.WriteData(data);
}

void RemoteDiff::OnThreadExit( ThreadExitEvent &event, bool firstTime )
{
    if (!firstTime) return;
}
