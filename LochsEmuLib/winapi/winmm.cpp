#include "stdafx.h"
#include "winapi.h"
#include "processor.h"
#include "instruction.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Winmm_waveOutClose(Processor *cpu)
{
    RET_VALUE = (u32) waveOutClose(
        (HWAVEOUT)      PARAM(0)
        );
    return 1;
}

uint Winmm_waveOutOpen(Processor *cpu)
{
    RET_VALUE = (u32) waveOutOpen(
        (LPHWAVEOUT)    PARAM_PTR(0),
        (uint)          PARAM(1),
        (LPCWAVEFORMATEX)   PARAM_PTR(2),
        (DWORD_PTR)     PARAM(3),
        (DWORD_PTR)     PARAM(4),
        (DWORD)         PARAM(5)
        );
    return 6;
}

uint Winmm_waveOutGetPosition(Processor *cpu)
{
    RET_VALUE = (u32) waveOutGetPosition(
        (HWAVEOUT)      PARAM(0),
        (LPMMTIME)      PARAM_PTR(1),
        (uint)          PARAM(2)
        );
    return 3;
}

uint Winmm_waveOutPause(Processor *cpu)
{
    RET_VALUE = (u32) waveOutPause(
        (HWAVEOUT)      PARAM(0)
        );
    return 1;
}

uint Winmm_waveOutPrepareHeader(Processor *cpu)
{
    RET_VALUE = (u32) waveOutPrepareHeader(
        (HWAVEOUT)      PARAM(0),
        (LPWAVEHDR)     PARAM_PTR(1),
        (UINT)          PARAM(2)
        );
    return 3;
}

uint Winmm_waveOutReset(Processor *cpu)
{
    RET_VALUE = (u32) waveOutReset(
        (HWAVEOUT)      PARAM(0)
        );
    return 1;
}

uint Winmm_waveOutRestart(Processor *cpu)
{
    RET_VALUE = (u32) waveOutRestart(
        (HWAVEOUT)      PARAM(0)
        );
    return 1;
}

uint Winmm_waveOutUnprepareHeader(Processor *cpu)
{
    RET_VALUE = (u32) waveOutUnprepareHeader(
        (HWAVEOUT)      PARAM(0),
        (LPWAVEHDR)     PARAM_PTR(1),
        (UINT)          PARAM(2)
        );
    return 3;
}

uint Winmm_waveOutWrite(Processor *cpu)
{
    RET_VALUE = (u32) waveOutWrite(
        (HWAVEOUT)      PARAM(0),
        (LPWAVEHDR)     PARAM_PTR(1),
        (UINT)          PARAM(2)
        );
    return 3;
}

END_NAMESPACE_LOCHSEMU()