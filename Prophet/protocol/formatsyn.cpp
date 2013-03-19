#include "stdafx.h"
#include "formatsyn.h"


FormatSyn::FormatSyn( MessageManager *msgmgr )
    : m_msgmgr(msgmgr)
{

}

FormatSyn::~FormatSyn()
{

}

void FormatSyn::Initialize()
{

}

void FormatSyn::OnMessageBegin( MessageBeginEvent &event )
{

}

void FormatSyn::OnMessageEnd( MessageEndEvent &event )
{
    // report message format & clear
}

void FormatSyn::Serialize( Json::Value &root ) const 
{

}

void FormatSyn::Deserialize( Json::Value &root )
{

}
