#pragma once
 
#ifndef __PROPHET_MEMREGION_H__
#define __PROPHET_MEMREGION_H__
 
#include "prophet.h"

struct MemRegion {
    u32 Addr;
    u32 Len;

    MemRegion() { Addr = Len = 0; }
    MemRegion(u32 addr, u32 len) {
        Addr = addr; Len = len;
    }

    bool Contains(u32 addr) const;
    bool Contains(const MemRegion &r) const;
};


#endif // __PROPHET_MEMREGION_H__