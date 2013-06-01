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
    //Message(u32 addr, int len);
    Message(u32 addr, int len, cpbyte data, Message *parent);
    virtual ~Message();

    int         Size() const { return (int) m_region.Len; }
    u32         Base() const { return m_region.Addr; }
    void        SetTraceRange(int beginIncl, int endIncl);
    MessageByte Get(int n) const { Assert(n >= 0 && n < Size()); return m_data[n]; }
    void        SetID(int id) { m_id = id; }
    int         GetID() const { return m_id; }
    //std::string ToString() const;
    MemRegion   GetRegion() const { return m_region; }
    Message *   GetParent() const { return m_parent; }
    int         GetTraceBegin() const { return m_traceBegin; }
    int         GetTraceEnd() const { return m_traceEnd; }

    void        Analyze(MessageManager *msgmgr, const RunTrace &trace);

private:
    int     m_id;
    int     m_traceBegin, m_traceEnd;
    MemRegion       m_region;
    MessageByte *   m_data;
    Message *       m_parent;
    std::vector<Message *>  m_children;

    MessageAccessLog *m_accesslog;
    MessageTree     *m_fieldTree;
};

#endif // __PROPHET_PROTOCOL_MESSAGE_H__