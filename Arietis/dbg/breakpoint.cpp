#include "stdafx.h"
#include "breakpoint.h"

Breakpoint::Breakpoint() 
    : m_module(0), m_offset(0), m_desc("invalid")
{
}

Breakpoint::Breakpoint( u32 module, u32 offset, const std::string &desc )
    : m_module(module), m_offset(offset), m_desc(desc)
{
}

void Breakpoint::Serialize( Json::Value &root ) const
{
    root["module"]  = m_module;
    root["offset"]  = m_offset;
    root["desc"]    = m_desc;
}

void Breakpoint::Deserialize( Json::Value &root )
{
    m_module        = root.get("module", 0).asUInt();
    m_offset        = root.get("offset", 0).asUInt();
    m_desc          = root.get("desc", "invalid").asString();
}
