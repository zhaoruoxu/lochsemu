#include "stdafx.h"
#include "message.h"


Message::Message( int len )
    : m_length(len)
{
    m_data = new MessageByte[m_length];
}

Message::Message( int len, cpbyte data )
    : m_length(len)
{
    m_data = new MessageByte[m_length];
    for (int i = 0; i < m_length; i++)
        m_data[i].Data = data[i];
}

Message::~Message()
{
    SAFE_DELETE_ARRAY(m_data);
}
