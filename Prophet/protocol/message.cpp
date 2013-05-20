#include "stdafx.h"
#include "message.h"

const char *FieldFormatName[] = {
    "unknown", "separator", "keyword", "length", "fixed_length", "var_length", NULL
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

std::string Message::ToString() const
{
    char buf[4096];
    int i = 0;
    for (; i < m_length; i++)
        buf[i] = m_data[i].Data;
    buf[i] = '\0';
    return std::string(buf);
}
