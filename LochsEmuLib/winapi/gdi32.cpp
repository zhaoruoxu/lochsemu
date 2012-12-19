#include "stdafx.h"
#include "winapi.h"
#include "processor.h"
#include "instruction.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Gdi32_CreateFontA(Processor *cpu)
{
    RET_VALUE = (u32) CreateFontA(
        (int)       PARAM(0),
        (int)       PARAM(1),
        (int)       PARAM(2),
        (int)       PARAM(3),
        (int)       PARAM(4),
        (DWORD)     PARAM(5),
        (DWORD)     PARAM(6),
        (DWORD)     PARAM(7),
        (DWORD)     PARAM(8),
        (DWORD)     PARAM(9),
        (DWORD)     PARAM(10),
        (DWORD)     PARAM(11),
        (DWORD)     PARAM(12),
        (LPCSTR)    PARAM_PTR(13)
        );
    RET_PARAMS(14);
}

uint Gdi32_CreatePen(Processor *cpu)
{	
	RET_VALUE =  (u32) CreatePen(
		(int)	PARAM(0),
		(int)	PARAM(1),
		(COLORREF)	PARAM(2)
		);
	RET_PARAMS(3);
}
uint Gdi32_CreateSolidBrush(Processor *cpu)
{
	RET_VALUE = (u32) CreateSolidBrush(
		(COLORREF)	PARAM(0)
		);
	RET_PARAMS(1);
}

uint Gdi32_GetStockObject(Processor *cpu)
{
    RET_VALUE = (u32) GetStockObject(
        (int)   PARAM(0)
        );
    RET_PARAMS(1);
}

uint Gdi32_GetTextMetricsA(Processor *cpu)
{
    RET_VALUE = (u32) GetTextMetricsA(
        (HDC)       PARAM(0),
        (LPTEXTMETRICA)     PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Gdi32_RoundRect(Processor *cpu)
{
	RET_VALUE = (u32) RoundRect(
		(HDC)	PARAM(0),
		(int)	PARAM(1),
		(int)	PARAM(2),
		(int)	PARAM(3),
		(int)	PARAM(4),
		(int)	PARAM(5),
		(int)	PARAM(6)
		);
	RET_PARAMS(7);
}

uint Gdi32_SelectObject(Processor *cpu)
{
    RET_VALUE = (u32) SelectObject(
        (HDC)       PARAM(0),
        (HGDIOBJ)   PARAM(1)
        );
    RET_PARAMS(2);
}

uint Gdi32_SetBkColor(Processor *cpu)
{
	RET_VALUE = (u32) SetBkColor(
		(HDC)	PARAM(0),
		(COLORREF)	PARAM(1)
		);
	RET_PARAMS(2);
}

uint Gdi32_SetBkMode(Processor *cpu)
{
	RET_VALUE = (u32) SetBkMode(
		(HDC)	PARAM(0),
		(int)	PARAM(1)
		);
	RET_PARAMS(2);
}

uint Gdi32_SetTextColor(Processor *cpu)
{
    RET_VALUE = (u32) SetTextColor(
        (HDC)       PARAM(0),
        (COLORREF)  PARAM(1)
        );
    RET_PARAMS(2);
}

END_NAMESPACE_LOCHSEMU()