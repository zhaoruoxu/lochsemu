#include "stdafx.h"
#include "callback.h"
#include "emulator.h"
#include "process.h"
#include "processor.h"
#include "win32.h"
#include "pluginmgr.h"

BEGIN_NAMESPACE_LOCHSEMU()

LRESULT CALLBACK LxWndProcProxy( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    DWORD id = GetCurrentThreadId();
    Processor *p = LxEmulator.GetProcessorByThreadID(id);
    if (p == NULL) return 0;
    p->PushContext();
    p->Push32((u32) lParam);
    p->Push32((u32) wParam);
    p->Push32((u32) message);
    p->Push32((u32) hwnd);
    p->Push32((u32) TERMINATE_EIP);
    p->RunCallback(LX_CALLBACK_WNDPROC);
    LRESULT lr = (LRESULT) p->EAX;
    p->PopContext();
    return lr;
}

BOOL WINAPI LxHandlerRoutine( DWORD CtrlType )
{
    DWORD id = GetCurrentThreadId();
    Processor *p = LxEmulator.GetProcessorByThreadID(id);
    if (p == NULL) return FALSE;
    p->PushContext();
    p->Push32(CtrlType);
    p->Push32(TERMINATE_EIP);
    p->RunCallback(LX_CALLBACK_CONSOLE_CTRL_HANDLER);
    BOOL b = (BOOL) p->EAX;
    p->PopContext();
    return b;
}

BOOL CALLBACK LxWndEnumProc( HWND hwnd, LPARAM lParam )
{
    DWORD id = GetCurrentThreadId();
    Processor *p = LxEmulator.GetProcessorByThreadID(id);
    if (p == NULL) return FALSE;
    p->PushContext();
    p->Push32(lParam);
    p->Push32((u32) hwnd);
    p->Push32(TERMINATE_EIP);
    p->RunCallback(LX_CALLBACK_WNDENUMPROC);
    BOOL b = (BOOL) p->EAX;
    p->PopContext();
    return b;
}

INT_PTR CALLBACK LxDlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    DWORD id = GetCurrentThreadId();
    Processor *p = LxEmulator.GetProcessorByThreadID(id);
    if (p == NULL) return 0;
    p->PushContext();
    p->Push32(lParam);
    p->Push32(wParam);
    p->Push32(msg);
    p->Push32((u32) hWnd);
    p->Push32(TERMINATE_EIP);
    p->RunCallback(LX_CALLBACK_DLGPROC);
    INT_PTR i = (INT_PTR) p->EAX;
    p->PopContext();
    return i;
}

VOID NTAPI LxFlsCallbackFunction( PVOID lpFlsData )
{
    DWORD id = GetCurrentThreadId();
    Processor *p = LxEmulator.GetProcessorByThreadID(id);
    if (p == NULL) return;
    p->PushContext();
    p->Push32((u32) lpFlsData);
    p->Push32(TERMINATE_EIP);
    p->RunCallback(LX_CALLBACK_FLS);
    p->PopContext();
}

LONG WINAPI LxTopLevelExceptionFilter( struct _EXCEPTION_POINTERS *ExceptionInfo )
{
    DWORD id = GetCurrentThreadId();
    Processor *p = LxEmulator.GetProcessorByThreadID(id);
    if (p == NULL) return 0;
    p->PushContext();
    p->Push32((u32) ExceptionInfo);
    p->Push32(TERMINATE_EIP);
    p->RunCallback(LX_CALLBACK_TOPLEVEL_EXCEPTION_FILTER);
    LONG l = (LONG) p->EAX;
    p->PopContext();
    return l;
}

BOOL CALLBACK LxInitOnceCallback(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context)
{
    DWORD id = GetCurrentThreadId();
    Processor *p = LxEmulator.GetProcessorByThreadID(id);
    if (p == NULL) return 0;
    p->PushContext();
    p->Push32((u32) Context);
    p->Push32((u32) Parameter);
    p->Push32((u32) InitOnce);
    p->Push32(TERMINATE_EIP);
    p->RunCallback(LX_CALLBACK_INITONCE);
    BOOL retval = (BOOL) p->EAX;
    p->PopContext();
    return retval;
}


END_NAMESPACE_LOCHSEMU()