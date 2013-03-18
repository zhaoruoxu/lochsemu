#include "stdafx.h"
#include "message.h"


Message::Message( int len )
    : m_length(len)
{
    m_data = new MessageByte[m_length];
}

Message::~Message()
{
    SAFE_DELETE_ARRAY(m_data);
}
