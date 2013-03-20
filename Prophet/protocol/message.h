#pragma once
 
#ifndef __PROPHET_PROTOCOL_MESSAGE_H__
#define __PROPHET_PROTOCOL_MESSAGE_H__

#include "prophet.h"

enum FieldFormat {
    Unknown     = 0,        // lower number with high priority
    Separator,
    Keyword,
    Length,
    FixedLen,
    VariableLen,
    //      lowest priority

    Total,  // total number of formats
};

const char *FieldFormatName[];

struct MessageByte {
    FieldFormat     Format;
    byte            Data;

    MessageByte() : Format(Unknown), Data(0)
    {
    }
};

class Message {
public:
    Message(int len, u32 addr);
    Message(int len, u32 addr, cpbyte data);
    virtual ~Message();

    int         Size() const { return m_length; }
    u32         Base() const { return m_baseAddr; }
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
    u32             m_baseAddr;
    int             m_length;
    MessageByte *   m_data;
};

#endif // __PROPHET_PROTOCOL_MESSAGE_H__