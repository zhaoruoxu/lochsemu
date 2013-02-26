#include "stdafx.h"
#include "archive.h"

Archive::Archive()
{
    m_breakpoints.emplace_back(10, 20, "test");
}

Archive::~Archive()
{

}

void Archive::Serialize( Json::Value &root ) const
{
    Json::Value bps;
    for (const Breakpoint &bp : m_breakpoints) {
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
            m_breakpoints.push_back(bp);
        }
    }
}

