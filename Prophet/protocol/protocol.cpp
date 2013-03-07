#include "stdafx.h"
#include "protocol.h"

Protocol::Protocol( ProEngine *engine )
    : m_engine(engine), m_pretaint(this)
{
    m_enabled = true;
}

Protocol::~Protocol()
{

}

void Protocol::Initialize()
{

}

void Protocol::OnWinapiPostCall( WinapiPostCallEvent &event )
{
    m_pretaint.OnWinapiPostCall(event);
}

void Protocol::Serialize( Json::Value &root ) const 
{
    root["enabled"] = m_enabled;
}

void Protocol::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();
}
