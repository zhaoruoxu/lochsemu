#include "stdafx.h"
#include "archive.h"

Archive::Archive() : m_mutex(false)
{
    //Breakpoints.emplace_back(10, 20, "test", false);
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

    root["breakpoints"] = bps;
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
}

