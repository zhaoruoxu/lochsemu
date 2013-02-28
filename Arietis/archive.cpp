#include "stdafx.h"
#include "archive.h"

Archive::Archive()
{
    IsTracerEnabled     = false;
    SkipDllEntries      = true;
    BreakOnCRTEntry     = true;
    IsTaintEnabled      = false;
}

Archive::~Archive()
{

}

void Archive::Serialize( Json::Value &root ) const
{
    Json::Value bps;
    for (const Breakpoint &bp : Breakpoints) {
        Json::Value b;
        bp.Serialize(b);
        bps.append(b);
    }

    root["breakpoints"]         = bps;
    root["is_tracer_enabled"]   = IsTracerEnabled;
    root["skip_dll_entries"]    = SkipDllEntries;
    root["break_on_crt_entry"]  = BreakOnCRTEntry;
    //root["is_taint_enabled"]    = IsTaintEnabled;
}

void Archive::Deserialize( Json::Value &root )
{
    Json::Value bps = root["breakpoints"];
    if (!bps.empty()) {
        for (uint i = 0; i < bps.size(); i++) {
            Breakpoint bp;
            bp.Deserialize(bps[i]);
            Breakpoints.push_back(bp);
        }
    }

    IsTracerEnabled     = root.get("is_tracer_enabled", false).asBool();
    SkipDllEntries      = root.get("skip_dll_entries", true).asBool();
    BreakOnCRTEntry     = root.get("break_on_crt_entry", true).asBool();
    //IsTaintEnabled      = root.get("is_taint_enabled", false).asBool();
}

