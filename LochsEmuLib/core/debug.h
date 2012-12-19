#pragma once

#ifndef __CORE_DEBUG_H__
#define __CORE_DEBUG_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

struct DebugContext {
    bool Debugging;
    u32 MemReadOffset;
    u32 MemWriteOffset;
    u32 EBP;
};

//extern DebugContext LxDebugContext;

void LX_API LochsDebuggingRoutine();

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_DEBUG_H__
