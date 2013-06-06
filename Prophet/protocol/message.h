#pragma once
 
#ifndef __PROPHET_PROTOCOL_MESSAGE_H__
#define __PROPHET_PROTOCOL_MESSAGE_H__

#include "prophet.h"
#include "memregion.h"

enum MessageType {
    MESSAGE_ASCII,
    MESSAGE_BINARY,
};

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


struct AlgParam {
    std::string Type;
    MemRegion Mem;
    pbyte Data;

    AlgParam(const std::string &t, const MemRegion &r, cpbyte d);
    ~AlgParam();
};

struct AlgTag {
    std::string AlgName;
    std::string Description;
    std::vector<AlgParam *> Params;

    void DumpDot(File &f) const;

    AlgTag(const std::string &name, const std::string &desc);
    ~AlgTag();
};

class Message {
public:
    //Message(u32 addr, int len);
    Message(const MemRegion &r, cpbyte data);
    Message(const MemRegion &r, cpbyte data, Message *parent, const MemRegion &pr, AlgTag *tag, bool clearNode);
    virtual ~Message();

    int         Size() const { return (int) m_region.Len; }
    u32         Base() const { return m_region.Addr; }
    void        SetTraceRange(int beginIncl, int endIncl);
    byte        Get(int n) const { Assert(n >= 0 && n < Size()); return m_data[n]; }
    cpbyte      GetRaw() const { return m_data; }
    void        SetID(int id);
    int         GetID() const { return m_id; }
    //std::string ToString() const;
    MemRegion   GetRegion() const { return m_region; }
    MemRegion   GetParentRegion() const { return m_parentRegion; }
    Message *   GetParent() const { return m_parent; }
    int         GetTraceBegin() const { return m_traceBegin; }
    int         GetTraceEnd() const { return m_traceEnd; }
    std::string GetName() const { return GetTypeString() + m_name; }
    MessageTree *GetTree() const { return m_fieldTree; }
    //const MessageTree *GetTree() const { return m_fieldTree; }
    AlgTag *    GetTag() const { return m_tag; }
    void        DumpTree(File &f) const;
    bool        SearchData(cpbyte p, int len, MemRegion &r);

    void        Analyze(MessageManager *msgmgr, const RunTrace &trace);
    void        AnalyzeAll(MessageManager *msgmgr, const RunTrace &trace);
    void        Insert(Message *msg);
    MessageType GetType() const { return m_type; }
    std::string GetTypeString() const;
private:
    void        ResolveType();
private:
    int     m_id;
    int     m_traceBegin, m_traceEnd;
    MemRegion       m_region;
    pbyte   m_data;
    Message *       m_parent;
    MemRegion       m_parentRegion;
    std::vector<Message *>  m_children;
    std::string     m_name;
    MessageAccessLog *m_accesslog;
    MessageTree     *m_fieldTree;
    AlgTag *        m_tag;
    bool    m_clearNode;
    MessageType     m_type;
};

#endif // __PROPHET_PROTOCOL_MESSAGE_H__