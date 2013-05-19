#include "stdafx.h"
#include "analyzer.h"
#include "engine.h"
#include "event.h"


ProtocolAnalyzer::ProtocolAnalyzer( Protocol *protocol, const std::string &name )
    : m_protocol(protocol), m_name(name)
{
    m_enabled = true;
}

ProtocolAnalyzer::~ProtocolAnalyzer()
{

}

void ProtocolAnalyzer::Serialize( Json::Value &root ) const 
{
    root["enabled"] = m_enabled;
}

void ProtocolAnalyzer::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();
}

// bool ProtocolAnalyzer::Compare( const ProtocolAnalyzer *lhs, const ProtocolAnalyzer *rhs )
// {
//     return lhs->GetExecOrder() < rhs->GetExecOrder();
// }
