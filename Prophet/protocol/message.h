#pragma once
 
#ifndef __PROPHET_PROTOCOL_MESSAGE_H__
#define __PROPHET_PROTOCOL_MESSAGE_H__

#include "prophet.h"
#include "memregion.h"

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
    Message(u32 addr, int len);
    Message(u32 addr, int len, cpbyte data);
    virtual ~Message();

    int         Size() const { return (int) m_region.Len; }
    u32         Base() const { return m_region.Addr; }
    MessageByte&    operator[](int index) 
    {
        Assert(index >= 0 && index < Size());
        return m_data[index];
    }

    const MessageByte &operator[](int index) const
    {
        Assert(index >= 0 && index < Size());
        return m_data[index];
    }

    std::string     ToString() const;
    MemRegion       GetRegion() const { return m_region; }

private:
    MemRegion       m_region;
    MessageByte *   m_data;
};

#endif // __PROPHET_PROTOCOL_MESSAGE_H__