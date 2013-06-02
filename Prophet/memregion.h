#pragma once
 
#ifndef __PROPHET_MEMREGION_H__
#define __PROPHET_MEMREGION_H__
 
#include "prophet.h"
#include "protocol/taint/taint.h"

struct MemRegion {
    u32 Addr;
    u32 Len;

    MemRegion() { Addr = Len = 0; }
    MemRegion(u32 addr, u32 len) {
        Addr = addr; Len = len;
    }

    bool operator==(const MemRegion &r) const {
        return Addr == r.Addr && Len == r.Len;
    }
    bool operator!=(const MemRegion &r) const {
        return Addr != r.Addr || Len != r.Len;
    }

    bool TryMerge(const MemRegion &r) {
        if (Addr + Len != r.Addr) return false;
        Len += r.Len;
        return true;
    }

    bool CanMerge(const MemRegion &r) const {
        return Addr + Len == r.Addr;
    }

    MemRegion SubRegion(const TaintRegion &t) const {
        Assert(t.Offset + t.Len <= (int) Len);
        return MemRegion(Addr + t.Offset, t.Len);
    }

    bool Contains(u32 addr) const;
    bool Contains(const MemRegion &r) const;
};


#endif // __PROPHET_MEMREGION_H__