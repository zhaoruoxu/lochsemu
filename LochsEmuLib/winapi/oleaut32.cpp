#include "stdafx.h"
#include "winapi.h"
#include "processor.h"
#include "emulator.h"
#include "heap.h"
#include "process.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Oleaut32_VariantClear(Processor *cpu)
{
	RET_VALUE = (u32) VariantClear(
		(VARIANTARG *)	PARAM_PTR(0));
	RET_PARAMS(1);
}

uint Oleaut32_9_VariantClear(Processor *cpu)
{
	RET_VALUE = (u32) VariantClear(
		(VARIANTARG *)	PARAM_PTR(0));
	RET_PARAMS(1);
}

uint Oleaut32_VariantCopy(Processor *cpu)
{
    RET_VALUE = (u32) VariantCopy(
        (VARIANTARG *)  PARAM_PTR(0),
        (const VARIANTARG *)    PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Oleaut32_SysAllocString(Processor *cpu)
{
	RET_VALUE = (u32) SysAllocString(
		(const OLECHAR *)	PARAM_PTR(0));
	RET_PARAMS(1);
}

uint Oleaut32_SysAllocStringByteLen(Processor *cpu)
{
	RET_VALUE = (u32) SysAllocStringByteLen(
		(LPCSTR)	PARAM_PTR(0),
		(UINT)		PARAM(1));
	RET_PARAMS(2);
}

uint Oleaut32_SysFreeString(Processor *cpu)
{
    SysFreeString(
        (BSTR)      PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

END_NAMESPACE_LOCHSEMU()