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

    MessageByte() : Format(Unknown) {

    }
};

class Message {
public:
    Message(int len);
    virtual ~Message();

private:
    int             m_length;
    MessageByte *   m_data;
};

#endif // __PROPHET_PROTOCOL_MESSAGE_H__