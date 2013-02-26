#include "stdafx.h"
#include "breakpoint.h"

Breakpoint::Breakpoint() 
    : Module(0), Offset(0), Desc("invalid"), 
    Address(0), ModuleName("invalid")
{
}

Breakpoint::Breakpoint( u32 module, u32 offset, const std::string &desc, bool enabled )
    : Module(module), Offset(offset), Desc(desc), Enabled(enabled),
    Address(0), ModuleName("invalid")
{
}

void Breakpoint::Serialize( Json::Value &root ) const
{
    root["module"]  = Module;
    root["offset"]  = Offset;
    root["desc"]    = Desc;
    root["enabled"] = Enabled;
}

void Breakpoint::Deserialize( Json::Value &root )
{
    Module        = root.get("module", 0).asUInt();
    Offset        = root.get("offset", 0).asUInt();
    Desc          = root.get("desc", "invalid").asString();
    Enabled       = root.get("enabled", true).asBool();
}
