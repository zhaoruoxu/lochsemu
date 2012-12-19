#pragma once

#ifndef __CORE_CALLBACK_H__
#define __CORE_CALLBACK_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define LX_CALLBACK_WNDPROC                 0
#define LX_CALLBACK_CONSOLE_CTRL_HANDLER    1
#define LX_CALLBACK_WNDENUMPROC             2
#define LX_CALLBACK_DLGPROC                 3
#define LX_CALLBACK_FLS                     4
#define LX_CALLBACK_TOPLEVEL_EXCEPTION_FILTER   5
#define LX_CALLBACK_INITONCE                6
    
#define LX_CALLBACKS                        7

LRESULT CALLBACK    LxWndProcProxy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL WINAPI         LxHandlerRoutine(DWORD CtrlType);
BOOL CALLBACK       LxWndEnumProc(HWND hwnd, LPARAM lParam);
INT_PTR CALLBACK    LxDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
VOID NTAPI          LxFlsCallbackFunction(PVOID lpFlsData);
LONG WINAPI         LxTopLevelExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo);
BOOL CALLBACK       LxInitOnceCallback(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context);

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_CALLBACK_H__
