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

private:
    int             m_length;
    MessageByte *   m_data;
};

#endif // __PROPHET_PROTOCOL_MESSAGE_H__