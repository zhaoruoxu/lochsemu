#pragma once
 
#ifndef __PROPHET_PROTOCOL_MESSAGE_H__
#define __PROPHET_PROTOCOL_MESSAGE_H__

#include "prophet.h"

enum FieldFormat {
    Unknown     = 0,
    Length,
    Separator,
    Keyword,
    FixedLen,
    VariableLen,
};

struct MessageByte {
    FieldFormat     Format;
    byte            Data;

    MessageByte() : Format(Unknown), Data(0)
    {
    }
};

class Message {
public:
    Message(int len);
    Message(int len, cpbyte data);
    virtual ~Message();

    int         Size() const { return m_length; }
    MessageByte&    operator[](int index) 
    {
        Assert(index >= 0 && index < m_length);
        return m_data[index];
    }

    const MessageByte &operator[](int index) const
    {
        Assert(index >= 0 && index < m_length);
        return m_data[index];
    }

private:
    int             m_length;
    MessageByte *   m_data;
};

#endif // __PROPHET_PROTOCOL_MESSAGE_H__