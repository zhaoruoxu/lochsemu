#include "stdafx.h"
#include "analyzer.h"
#include "engine.h"
#include "event.h"


ProtocolAnalyzer::ProtocolAnalyzer( int execOrder, const std::string &name, uint handlerFlags )
    : m_execOrder(execOrder), m_name(name), m_handlerFlags(handlerFlags)
{
    m_enabled = true;
    m_protocol = g_engine.GetProtocol();
    m_protocol->RegisterAnalyzer(this);
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

bool ProtocolAnalyzer::Compare( const ProtocolAnalyzer *lhs, const ProtocolAnalyzer *rhs )
{
    return lhs->GetExecOrder() < rhs->GetExecOrder();
}
