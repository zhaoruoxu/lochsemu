#include "stdafx.h"
#include "exception.h"
#include "processor.h"
#include "instruction.h"
#include "config.h"

BEGIN_NAMESPACE_LOCHSEMU()


ExceptionManager::ExceptionManager()
{

}

ExceptionManager::~ExceptionManager()
{

}

void ExceptionManager::Initialize( Processor *cpu )
{
    Assert(cpu);
    m_cpu       = cpu;
    m_enabled   = LxConfig.GetInt("Emulator", "EnableExceptions", 0) != 0;
}

void ExceptionManager::Raise( DWORD code, DWORD flags, DWORD numParams, LPVOID params )
{
    if (!m_enabled) {
        LxFatal("Exception %08X at [%08x] %s\n", code, (u32) m_cpu->CurrentInst()->Main.VirtualAddr,
            m_cpu->CurrentInst()->Main.CompleteInstr);
        return;
    }

    u32 contextAddress = ConstructContext();
    u32 exceptionRecordAddress = ConstructExceptionRecord(code, flags, numParams, params);

    u32 exceptionRegistration = m_cpu->MemRead32(0, LX_REG_FS);
    u32 handler = m_cpu->MemRead32(exceptionRegistration + 4);

    EXCEPTION_DISPOSITION disp = (EXCEPTION_DISPOSITION) RunExceptionHandler(
        handler, exceptionRecordAddress, contextAddress, 0);
    if (disp != ExceptionContinueExecution) {
        LxFatal("ExceptionDisposition is not ExceptionContinueExecution\n");
        // TODO: may continue search
    }

    RemoveExceptionRecord(exceptionRecordAddress);
    RestoreContext(contextAddress);
}

u32 ExceptionManager::ConstructContext( void )
{
    CONTEXT context;
    ZeroMemory(&context, sizeof(CONTEXT));

    m_cpu->ToContext(&context);
    m_cpu->Push(sizeof(CONTEXT), (cpbyte) &context);
    return m_cpu->ESP;
}

u32 ExceptionManager::ConstructExceptionRecord( DWORD code, DWORD flags, DWORD nParams, 
                                                LPVOID params, EXCEPTION_RECORD *chain)
{
    EXCEPTION_RECORD record;
    ZeroMemory(&record, sizeof(EXCEPTION_RECORD));

    if (chain) {
        LxWarning("Chained ExceptionRecord is not supported\n");
        // TODO: Nested ExceptionRecord
    }

    record.ExceptionCode        = code;
    record.ExceptionFlags       = flags;
    record.ExceptionRecord      = chain;
    record.ExceptionAddress     = (PVOID) m_cpu->CurrentInst()->Main.VirtualAddr;
    record.NumberParameters     = nParams;
    if (nParams > 0) {
        LxWarning("how many parameters do you want??\n");
    }

    m_cpu->Push(sizeof(EXCEPTION_RECORD), (cpbyte) &record);
    return m_cpu->ESP;
}

u32 ExceptionManager::RunExceptionHandler( u32 entry, u32 recordAddr, u32 ctxAddr, u32 dispCtxAddr )
{

    /*
     *  Exception handler interface:
     *  
     *   EXCEPTION_DISPOSITION __cdecl _except_handler (
     *       _In_ struct _EXCEPTION_RECORD *_ExceptionRecord,
     *       _In_ void * _EstablisherFrame,
     *       _Inout_ struct _CONTEXT *_ContextRecord,
     *       _Inout_ void * _DispatcherContext
     *       );
     */

    u32 frame = m_cpu->MemRead32(0, LX_REG_FS);
    Assert(frame);
    m_cpu->Push32(dispCtxAddr);
    m_cpu->Push32(ctxAddr);
    m_cpu->Push32(frame);
    m_cpu->Push32(recordAddr);
    m_cpu->Push32((u32) TERMINATE_EIP);
    m_cpu->RunConditional(entry);
    return m_cpu->EAX;
}

void ExceptionManager::RestoreContext( u32 ctxAddress )
{
    CONTEXT context;
    if (ctxAddress != m_cpu->ESP) {
        LxFatal("Calling ExceptionManager::RestoreContext() with incorrect stack pointer!\n");
    }
    m_cpu->Pop(sizeof(CONTEXT), (pbyte) &context);
    m_cpu->FromContext(&context);
}

void ExceptionManager::RemoveExceptionRecord( u32 addr )
{
    EXCEPTION_RECORD record;
    if (addr != m_cpu->ESP) {
        LxFatal("Calling ExceptionManager::RemoveExceptionRecord() with incorrect stack pointer!\n");
    }
    m_cpu->Pop(sizeof(EXCEPTION_RECORD), (pbyte) &record);
    if (record.ExceptionRecord) {
        LxWarning("Nested EXCEPTION_RECORD not implemented\n");
    }
}

END_NAMESPACE_LOCHSEMU()