#include "stdafx.h"
#include "message.h"

const char *FieldFormatName[] = {
    "unknown", "separator", "keyword", "length", "fixed_length", "var_length", NULL
};

Message::Message( u32 addr, int len )
    : m_region(addr, len)
{
    m_data = new MessageByte[len];
}

Message::Message( u32 addr, int len, cpbyte data )
    : m_region(addr, len)
{
    m_data = new MessageByte[len];
    for (int i = 0; i < Size(); i++)
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
    for (; i < Size(); i++)
        buf[i] = m_data[i].Data;
    buf[i] = '\0';
    return std::string(buf);
}
