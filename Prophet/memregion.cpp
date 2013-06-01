#include "stdafx.h"
#include "memregion.h"

bool MemRegion::Contains( u32 addr ) const
{
    return Addr <= addr && addr < Addr + Len;
}

bool MemRegion::Contains( const MemRegion &r ) const
{
    return Addr <= r.Addr && r.Addr + r.Len <= Addr + Len;
}
