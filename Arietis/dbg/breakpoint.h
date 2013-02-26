#pragma once
 
#ifndef __ARIETIS_DBG_BREAKPOINT_H__
#define __ARIETIS_DBG_BREAKPOINT_H__
 
#include "Arietis.h"
#include "io.h"

struct Breakpoint : public ISerializable {

    Breakpoint();
    Breakpoint(u32 module, u32 offset, const std::string &desc, bool enabled);
    ~Breakpoint() {}

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

    // serialize
    u32         Module;
    u32         Offset;
    std::string Desc;
    bool        Enabled;

    // non-serialize
    u32         Address;
    std::string ModuleName;
};


 
#endif // __ARIETIS_DBG_BREAKPOINT_H__