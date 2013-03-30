
#include "stdafx.h"
#include "lochsemu.h"
#include "debug.h"

using namespace LochsEmu;

int _tmain(int argc, _TCHAR* argv[])
{
    LochsDebuggingRoutine();

    if (argc == 1) {
        printf("Usage: lochsemu  [exe file name]\n");
        return 0;
    } else {
        LxRun(argc - 1, argv + 1);
        //LxReset();
    }

	return 0;
}

