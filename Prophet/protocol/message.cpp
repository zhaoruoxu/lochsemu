#include "stdafx.h"
#include "message.h"

const char *FieldFormatName[] = {
    "unknown", "separator", "keyword", "length", "fixed_length", "variable", NULL
};

Message::Message( int len, u32 addr )
    : m_length(len), m_baseAddr(addr)
{
    m_data = new MessageByte[m_length];
}

Message::Message( int len, u32 addr, cpbyte data )
    : m_length(len), m_baseAddr(addr)
{
    m_data = new MessageByte[m_length];
    for (int i = 0; i < m_length; i++)
        m_data[i].Data = data[i];
}

Message::~Message()
{
    SAFE_DELETE_ARRAY(m_data);
}
