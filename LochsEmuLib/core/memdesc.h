#pragma once

#ifndef __CORE_PAGE_H__
#define __CORE_PAGE_H__

#include "lochsemu.h"
#include "section.h"

BEGIN_NAMESPACE_LOCHSEMU()

struct LX_API PageDesc {
    uint    Protect;
    uint    Characristics;

    PageDesc() 
    {
        Protect = PAGE_NOACCESS;
        Characristics = LX_CHR_RESERVED;
    }
    PageDesc(uint protect, uint chr)
    {
        Protect = protect;
        Characristics = chr;
    }
};

struct LX_API PageInfo : PageDesc {
    u32 address;

    PageInfo(u32 addr, const PageDesc &desc)
        : PageDesc(desc), address(addr) {}
};

struct LX_API SectionDesc {
    std::string     Desc;
    uint            Module;

    SectionDesc(): Desc("Unknown"), Module(LX_UNKNOWN_MODULE)
    {
    }

    SectionDesc(const std::string &d, uint m)
        : Desc(d), Module(m) 
    {
    }
};

struct LX_API SectionInfo : SectionDesc {
    u32     base;
    u32     size;

    SectionInfo(const SectionDesc &desc, u32 b, u32 s) 
        :SectionDesc(desc), base(b), size(s)
    { 
    }
};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_PAGE_H__
