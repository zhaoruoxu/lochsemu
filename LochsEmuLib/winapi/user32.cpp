#include "stdafx.h"
#include "winapi.h"
#include "processor.h"
#include "instruction.h"
#include "emulator.h"

BEGIN_NAMESPACE_LOCHSEMU()

static std::string MenuName;

uint User32_AppendMenuA(Processor *cpu)
{
    RET_VALUE = (u32) AppendMenuA(
        (HMENU)     PARAM(0),
        (UINT)      PARAM(1),
        (UINT_PTR)  PARAM(2),
        (LPCSTR)    PARAM_PTR(3)
        );
    RET_PARAMS(4);
}

uint User32_BeginPaint(Processor *cpu)
{
    RET_VALUE = (u32) BeginPaint(
        (HWND)  PARAM(0),
        (LPPAINTSTRUCT) PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint User32_CharUpperW(Processor *cpu)
{
	RET_VALUE = (u32) CharUpperW(
		(LPWSTR)	PARAM_PTR(0));
	RET_PARAMS(1);
}

uint User32_CreateDialogParamA(Processor *cpu)
{
	HINSTANCE hInstance = LxEmulator.TranslateModule((HMODULE)	PARAM(0));
	LPCSTR lpName;
	u32 p1 = PARAM(1);
	if (HIWORD(p1)) {
		lpName = (LPCSTR) PARAM_PTR(1);
	} else {
		lpName = (LPCSTR) p1;
	}
	cpu->SetCallbackEntry(LX_CALLBACK_DLGPROC, PARAM(3));
	RET_VALUE = (u32) CreateDialogParamA(
		(HINSTANCE)		hInstance,
		lpName,
		(HWND)			PARAM(2),
		LxDlgProc,
		(LPARAM)		PARAM(4)
		);
	RET_PARAMS(5);
}

uint User32_CreateWindowExA(Processor *cpu)
{
    DWORD dwExStyle = (DWORD) cpu->GetStackParam32(0);
    LPCSTR lpClassName = (LPCSTR) cpu->GetStackParamPtr32(1);
    LPCSTR lpWindowName = (LPCSTR) cpu->GetStackParamPtr32(2);
    DWORD dwStyle = (DWORD) cpu->GetStackParam32(3);
    int x = (int) cpu->GetStackParam32(4);
    int y = (int) cpu->GetStackParam32(5);
    int nWidth = (int) cpu->GetStackParam32(6);
    int nHeight = (int) cpu->GetStackParam32(7);
    HWND hWndParent = (HWND) cpu->GetStackParam32(8);
    HMENU hMenu = MenuName.empty() ? (HMENU) cpu->GetStackParam32(9) 
        : LoadMenuA(LxEmulator.GetRealHModule(), MenuName.c_str());
    HINSTANCE hInstance = (HINSTANCE) cpu->GetStackParam32(10);

    u32 val11 = (u32) PARAM_PTR(11);
    LxWarning("In User32.dll::CreateWindowExA(), lpParam is set to NULL\n");
    LPVOID lpParam = NULL;

    cpu->EAX = (u32) CreateWindowExA(dwExStyle, lpClassName, lpWindowName,
        dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    return 12;
}

uint User32_DeleteMenu(Processor *cpu)
{
    RET_VALUE = (u32) DeleteMenu(
        (HMENU)     PARAM(0),
        (UINT)      PARAM(1),
        (UINT)      PARAM(2)
        );
    RET_PARAMS(3);
}

uint User32_DestroyWindow(Processor *cpu)
{
    RET_VALUE = (u32) DestroyWindow(
        (HWND)      PARAM(0)
        );
    RET_PARAMS(1);
}

uint User32_DialogBoxParamA(Processor *cpu)
{
    HINSTANCE hInstance = LxEmulator.TranslateModule((HMODULE) PARAM(0));
	LPCSTR lpName;
	u32 p1 = PARAM(1);
	if (HIWORD(p1)) {
		lpName = (LPCSTR) PARAM_PTR(1);
	} else {
		lpName = (LPCSTR) p1;
	}
	cpu->SetCallbackEntry(LX_CALLBACK_DLGPROC, PARAM(3));
    RET_VALUE = (u32) DialogBoxParamA(
        (HINSTANCE)     hInstance,
        lpName,
        (HWND)          PARAM(2),
        LxDlgProc,
        (LPARAM)        PARAM(4)
        );
    RET_PARAMS(5);
}

uint User32_DispatchMessageA(Processor *cpu)
{
    RET_VALUE = (u32) DispatchMessageA((LPMSG)PARAM_PTR(0));
	RET_PARAMS(1);
}

uint User32_DefWindowProcA(Processor *cpu)
{
    HWND hWnd = (HWND) cpu->GetStackParam32(0);
    UINT msg = cpu->GetStackParam32(1);
    WPARAM wParam = (WPARAM) cpu->GetStackParam32(2);
    LPARAM lParam = (LPARAM) cpu->GetStackParam32(3);
    cpu->EAX = (u32) DefWindowProcA(hWnd, msg, wParam, lParam);
    return 4;
}

uint User32_DrawMenuBar(Processor *cpu)
{
    RET_VALUE = (u32) DrawMenuBar(
        (HWND)  PARAM(0)
        );
    RET_PARAMS(1);
}

uint User32_DrawTextA(Processor *cpu)
{
    RET_VALUE = (u32) DrawTextA(
        (HDC)       PARAM(0),
        (LPCSTR)    PARAM_PTR(1),
        (int)       PARAM(2),
        (LPRECT)    PARAM_PTR(3),
        (UINT)      PARAM(4)
        );
    RET_PARAMS(5);
}

uint User32_EndDialog(Processor *cpu)
{
    RET_VALUE = (u32) EndDialog(
        (HWND)      PARAM(0),
        (INT_PTR)   PARAM(1)
        );
    RET_PARAMS(2);
}

uint User32_EndPaint(Processor *cpu)
{
    RET_VALUE = (u32) EndPaint(
        (HWND)      PARAM(0),
        (const PAINTSTRUCT*)    PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint User32_EnumThreadWindows(Processor *cpu)
{
    cpu->SetCallbackEntry(LX_CALLBACK_WNDENUMPROC, (u32) PARAM(1) );
    RET_VALUE = (u32) EnumThreadWindows(
        (DWORD)     PARAM(0),
        LxWndEnumProc,
        (LPARAM)    PARAM(2)
        );
    RET_PARAMS(3);
}

uint User32_FillRect(Processor *cpu)
{
	RET_VALUE = (u32) FillRect(
		(HDC)	PARAM(0),
		(CONST RECT *)	PARAM_PTR(1),
		(HBRUSH)	PARAM(2)
		);
	RET_PARAMS(3);
}

uint User32_GetActiveWindow(Processor *cpu)
{
    RET_VALUE = (u32) GetActiveWindow();
    RET_PARAMS(0);
}

uint User32_GetDC(Processor *cpu)
{
    RET_VALUE = (u32) GetDC(
        (HWND)      PARAM(0)
        );
    RET_PARAMS(1);
}

uint User32_GetDlgItem(Processor *cpu)
{
    RET_VALUE = (u32)GetDlgItem(
        (HWND)      PARAM(0), 
        (int)       PARAM(1)
        );
    RET_PARAMS(2);
}

uint User32_GetDlgItemInt(Processor *cpu)
{
	RET_VALUE = (u32) GetDlgItemInt(
		(HWND)	PARAM(0),
		(int)	PARAM(1),
		(BOOL *)	PARAM_PTR(2),
		(BOOL)	PARAM(3)
		);
	RET_PARAMS(4);
}

uint User32_GetDlgItemTextA(Processor *cpu)
{

    RET_VALUE = (u32) GetDlgItemTextA(
        (HWND)      PARAM(0),
        (int)       PARAM(1),
        (LPSTR)    PARAM_PTR(2),
        (int)       PARAM(3)
        );
    RET_PARAMS(4);
}

uint User32_GetLastActivePopup(Processor *cpu)
{
    RET_VALUE = (u32) GetLastActivePopup(
        (HWND)      PARAM(0)
        );
    RET_PARAMS(1);
}

uint User32_GetMenu(Processor *cpu)
{
    RET_VALUE = (u32) GetMenu(
        (HWND)      PARAM(0)
        );
    RET_PARAMS(1);
}

uint User32_GetMessageA(Processor *cpu)
{
    LPMSG msg = (LPMSG) cpu->GetStackParamPtr32(0);
    HWND hWnd = (HWND) cpu->GetStackParam32(1);
    UINT wMin = cpu->GetStackParam32(2);
    UINT wMax = cpu->GetStackParam32(3);
    cpu->EAX = (u32) GetMessageA(msg, hWnd, wMin, wMax);
    return 4;
}

uint User32_GetProcessWindowStation(Processor *cpu)
{
    RET_VALUE = (u32) GetProcessWindowStation();
    RET_PARAMS(0);
}

uint User32_GetUserObjectInformationA(Processor *cpu)
{
    RET_VALUE = (u32) GetUserObjectInformationA(
        (HANDLE)    PARAM(0),
        (int)       PARAM(1),
        (PVOID)     PARAM_PTR(2),
        (DWORD)     PARAM(3),
        (LPDWORD)   PARAM_PTR(4)
        );
    RET_PARAMS(5);
}

uint User32_GetUserObjectInformationW(Processor *cpu)
{
	RET_VALUE = (u32) GetUserObjectInformationW(
		(HANDLE)	PARAM(0),
		(int)		PARAM(1),
		(PVOID)		PARAM_PTR(2),
		(DWORD)		PARAM(3),
		(LPDWORD)	PARAM_PTR(4)
		);
	RET_PARAMS(5);
}

uint User32_GetWindowTextA(Processor *cpu)
{
    RET_VALUE = (u32) GetWindowTextA(
        (HWND)      PARAM(0),
        (LPSTR)     PARAM_PTR(1),
        (int)       PARAM(2)
        );
    RET_PARAMS(3);
}

uint User32_GetWindowTextLengthA(Processor *cpu)
{
	RET_VALUE = (u32) GetWindowTextLengthA(
		(HWND)	PARAM(0)
		);
	RET_PARAMS(1);
}
uint User32_InvalidateRect(Processor *cpu)
{
    RET_VALUE = (u32) InvalidateRect(
        (HWND)      PARAM(0),
        (CONST RECT*)   PARAM_PTR(1),
        (BOOL)      PARAM(2)
        );
    RET_PARAMS(3);
}

uint User32_IsDlgButtonChecked(Processor *cpu)
{
	RET_VALUE = (u32) IsDlgButtonChecked(
		(HWND)	PARAM(0),
		(int)	PARAM(1)
		)				;
	RET_PARAMS(2);
}

uint User32_LoadCursorA(Processor *cpu)
{
    HINSTANCE hInstance = (HINSTANCE) cpu->GetStackParam32(0);
    LPCSTR lpName;
    u32 p1 = PARAM(1);
    if (HIWORD(p1)) {
        lpName = (LPCSTR) PARAM_PTR(1);
    } else {
        lpName = (LPCSTR) p1;
    }
    cpu->EAX = (u32) LoadCursorA(hInstance, lpName);
    return 2;
}

uint User32_LoadIconA(Processor *cpu)
{
    HINSTANCE hInstance = (HINSTANCE) cpu->GetStackParam32(0);
    hInstance = LxEmulator.TranslateModule(hInstance);
    LPCSTR lpName;
    u32 p1 = PARAM(1);
    if (HIWORD(p1)) {
        lpName = (LPCSTR) PARAM_PTR(1);
    } else {
        lpName = (LPCSTR) p1;
    }
    cpu->EAX = (u32) LoadIconA(hInstance, lpName);
    return 2;
}

uint User32_LoadMenuA(Processor *cpu)
{
    HINSTANCE hInstance = (HINSTANCE) PARAM(0);
    hInstance = LxEmulator.TranslateModule(hInstance);
    LPCSTR lpName;
    u32 p1 = PARAM(1);
    if (HIWORD(p1)) {
        lpName = (LPCSTR) PARAM_PTR(1);
    } else {
        lpName = (LPCSTR) p1;
    }
    RET_VALUE = (u32) LoadMenuA(hInstance, lpName);
    return 2;
}

uint User32_LoadStringA(Processor *cpu)
{
	HINSTANCE hInstance = (HINSTANCE) PARAM(0);
	hInstance = LxEmulator.TranslateModule(hInstance);
	RET_VALUE = LoadStringA(hInstance,
		(UINT)		PARAM(1),
		(LPSTR)	PARAM_PTR(2),
		(int)		PARAM(3)
		);
	RET_PARAMS(4);
}


uint User32_MessageBoxA( Processor *cpu )
{
    RET_VALUE = (u32) MessageBoxA(
        (HWND)      PARAM(0),
        (LPCSTR)    PARAM_PTR(1),
        (LPCSTR)    PARAM_PTR(2),
        (UINT)      PARAM(3)
        );
    return 4;
}

 uint User32_MessageBoxW(Processor *cpu)
 {
 	RET_VALUE = (u32) MessageBoxW(
 		(HWND)		PARAM(0),
 		(LPCWSTR) PARAM_PTR(1),
 		(LPCWSTR) PARAM_PTR(2),
 		(UINT)		PARAM(3)
 		);
 	RET_PARAMS(4);
 }

uint User32_PeekMessageA(Processor *cpu)
{
    RET_VALUE = (u32) PeekMessageA(
        (LPMSG)     PARAM_PTR(0),
        (HWND)      PARAM(1),
        (UINT)      PARAM(2),
        (UINT)      PARAM(3),
        (UINT)      PARAM(4)
        );
    RET_PARAMS(5);
}

uint User32_PostQuitMessage(Processor *cpu)
{
    UINT n = cpu->GetStackParam32(0);
    PostQuitMessage(n);
    return 0;
}

uint User32_RegisterClassA(Processor *cpu)
{
    WNDCLASSA *wnd = (WNDCLASSA *) PARAM_PTR(0);

    if (wnd->lpszMenuName)
        MenuName = std::string((char *) cpu->Mem->GetRawData((u32) wnd->lpszMenuName));

    cpu->SetCallbackEntry(LX_CALLBACK_WNDPROC, (u32) wnd->lpfnWndProc);
    wnd->lpfnWndProc = LxWndProcProxy;
    wnd->lpszClassName = wnd->lpszClassName == NULL ? NULL :
        (LPCSTR) cpu->Mem->GetRawData((u32) wnd->lpszClassName);
    wnd->lpszMenuName = wnd->lpszMenuName == NULL ? NULL :
        (LPCSTR) cpu->Mem->GetRawData((u32) wnd->lpszMenuName);
    RET_VALUE = (u32) RegisterClassA(wnd);
    RET_PARAMS(1);
}

uint User32_RegisterClassExA(Processor *cpu)
{
    WNDCLASSEXA * wnd = (WNDCLASSEXA *) cpu->GetStackParamPtr32(0);
    if (wnd->lpszMenuName)
        MenuName = std::string((char *) cpu->Mem->GetRawData((u32) wnd->lpszMenuName));

    cpu->SetCallbackEntry(LX_CALLBACK_WNDPROC, (u32) wnd->lpfnWndProc);
    wnd->lpfnWndProc = LxWndProcProxy;
    wnd->lpszClassName = wnd->lpszClassName == NULL ? NULL : 
        (LPCSTR) cpu->Mem->GetRawData((u32) wnd->lpszClassName);
    wnd->lpszMenuName = wnd->lpszMenuName == NULL ? NULL : 
        (LPCSTR) cpu->Mem->GetRawData((u32) wnd->lpszMenuName);
    cpu->EAX = (u32) RegisterClassExA(wnd);
    return 1;
}

uint User32_ReleaseCapture(Processor *cpu)
{
	RET_VALUE = (u32) ReleaseCapture();
	RET_PARAMS(0);
}

uint User32_ReleaseDC(Processor *cpu)
{
    RET_VALUE = (u32) ReleaseDC(
        (HWND)  PARAM(0),
        (HDC)   PARAM(1)
        );
    RET_PARAMS(2);
}

uint User32_ScrollWindow(Processor *cpu)
{
    RET_VALUE = (u32) ScrollWindow(
        (HWND)  PARAM(0),
        (int)   PARAM(1),
        (int)   PARAM(2),
        (CONST RECT *)  PARAM_PTR(3),
        (CONST RECT *)  PARAM_PTR(4)
        );
    return 5;
}

uint User32_SendDlgItemMessageA(Processor *cpu)
{
    UINT msg = (UINT) PARAM(2);
    WPARAM wParam = (WPARAM) PARAM(3);
    LPARAM lParam = (LPARAM) PARAM(4);

    LxWarning("SendDlgItemMessageA() may contain incorrect pointer\n");

    if (msg == CB_ADDSTRING) {
        lParam = (LPARAM) PARAM_PTR(4);
    }

    RET_VALUE = (u32) SendDlgItemMessageA(
        (HWND)      PARAM(0),
        (int)       PARAM(1),
        msg, wParam, lParam
        );
    return 5;
}

uint User32_SendMessageA(Processor *cpu)
{
	RET_VALUE = (u32) SendMessageA(
		(HWND)	PARAM(0),
		(UINT)	PARAM(1),
		(WPARAM)	PARAM(2),
		(LPARAM)	PARAM(3)
		);
	RET_PARAMS(4);
}

uint User32_SetDlgItemTextA(Processor *cpu)
{
	RET_VALUE = (u32) SetDlgItemTextA(
		(HWND)	PARAM(0),
		(int)	PARAM(1),
		(LPCSTR)	PARAM_PTR(2)
		);
	RET_PARAMS(3);
}

uint User32_SetFocus(Processor *cpu)
{
    RET_VALUE = (u32) SetFocus(
        (HWND)      PARAM(0)
        );
    RET_PARAMS(1);
}

uint User32_SetRect(Processor *cpu)
{
    LPRECT lpRect = (LPRECT) cpu->GetStackParamPtr32(0);
    int xLeft = (int) cpu->GetStackParam32(1);
    int yTop = (int) cpu->GetStackParam32(2);
    int xRight = (int) cpu->GetStackParam32(3);
    int yBottom = (int) cpu->GetStackParam32(4);
    cpu->EAX = (u32) SetRect(lpRect, xLeft, yTop, xRight, yBottom);
    return 5;
}

uint User32_SetScrollPos(Processor *cpu)
{
    RET_VALUE = (u32) SetScrollPos(
        (HWND)      PARAM(0),
        (int)       PARAM(1),
        (int)       PARAM(2),
        (BOOL)      PARAM(3)
        );
    RET_PARAMS(4);
}

uint User32_SetScrollRange(Processor *cpu)
{
    RET_VALUE = (u32) SetScrollRange(
        (HWND)      PARAM(0),
        (int)       PARAM(1),
        (int)       PARAM(2),
        (int)       PARAM(3),
        (BOOL)      PARAM(4)
        );
    RET_PARAMS(5);
}

uint User32_SetWindowLongA(Processor *cpu)
{
	RET_VALUE = (u32) SetWindowLongA(
		(HWND)      PARAM(0),
		(int)       PARAM(1),
		(LONG)      PARAM(2)
		);
	RET_PARAMS(3);
}

uint User32_SetWindowPos(Processor *cpu)
{
	RET_VALUE = (u32) SetWindowPos(
		(HWND)		PARAM(0),
		(HWND)		PARAM(1),
		(int)		PARAM(2),
		(int)		PARAM(3),
		(int)		PARAM(4),
		(int)		PARAM(5),
		(UINT)		PARAM(6)
		);
	RET_PARAMS(7);
}

uint User32_SetWindowTextA(Processor *cpu)
{
    RET_VALUE = (u32) SetWindowTextA(
        (HWND)      PARAM(0),
        (LPCSTR)    PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint User32_ShowWindow(Processor *cpu)
{
    HWND hWnd = (HWND) cpu->GetStackParam32(0);
    int nCmdShow = (int) cpu->GetStackParam32(1);
    cpu->EAX = (u32) ShowWindow(hWnd, nCmdShow);
    return 2;
}

uint User32_TranslateMessage(Processor *cpu)
{
    LPMSG msg = (LPMSG) cpu->GetStackParamPtr32(0);
    cpu->EAX = (u32) TranslateMessage(msg);
    return 1;
}


uint User32_UpdateWindow(Processor *cpu)
{
    HWND hWnd = (HWND) cpu->GetStackParam32(0);
    cpu->EAX = (u32) UpdateWindow(hWnd);
    return 1;
}

uint User32_ValidateRect(Processor *cpu)
{
    RET_VALUE = (u32) ValidateRect(
        (HWND)      PARAM(0),
        (CONST RECT*)   PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint User32_WinHelpA(Processor *cpu)
{
    RET_VALUE = (u32) WinHelpA(
        (HWND)      PARAM(0),
        (LPCSTR)    PARAM_PTR(1),
        (UINT)      PARAM(2),
        (ULONG_PTR) PARAM(3)
        );
    RET_PARAMS(4);
}

uint User32_wsprintfA(Processor *cpu)
{
    u32 ip = PARAM(-1);
    pbyte text = cpu->Mem->GetRawData(ip);
    Instruction inst;
    LxDecode(text, &inst, cpu->EIP);
    Assert(inst.Main.Inst.Category & ARITHMETIC_INSTRUCTION);
    uint nParamBytes = (u32) inst.Main.Inst.Immediat;

    LPSTR lpParam0 = (LPSTR) PARAM_PTR(0);
    LPCSTR lpParam1 = (LPSTR) PARAM_PTR(1);
    pbyte buf = new byte[nParamBytes - 8];  // minus two known params
    LPCSTR lpFormat = lpParam1;
    for (uint i = 2; i < nParamBytes/4; i++) {
        lpFormat = strstr(lpFormat, "%");
        if (NULL == lpFormat) break;
        lpFormat = lpFormat + 1;
        while ( isdigit(*lpFormat) ) lpFormat++;
        ((u32 *) buf)[i-2] = (*lpFormat == 's') ? (u32) PARAM_PTR(i) : (u32) PARAM(i);
    }
    RET_VALUE = (u32) vsprintf(lpParam0, lpParam1, (va_list) buf);
    SAFE_DELETE_ARRAY(buf);
    RET_PARAMS(0);      // cuz it's _cdcel
}

END_NAMESPACE_LOCHSEMU()