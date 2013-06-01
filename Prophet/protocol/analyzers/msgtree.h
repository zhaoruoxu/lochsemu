#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_MSGTREE_H__
#define __PROPHET_PROTOCOL_ANALYZERS_MSGTREE_H__
 
#include "msgaccess.h"

class MessageAccessComparator {
public:
    virtual bool Equals(const MessageAccess *l, const MessageAccess *r) = 0;
};

class StackHashComparator : public MessageAccessComparator {
public:
    virtual bool Equals(const MessageAccess *l, const MessageAccess *r) override;
};

typedef std::set<u32>   ExecutionHistory;

enum NodeFlag {
    TREENODE_PARALLEL = 1,

};

class MessageTreeNode {
    friend class MessageTree;
    friend class MessageTreeRefiner;
    friend class TokenizeRefiner;
    friend class ParallelFieldDetector;
public:
    MessageTreeNode(int l, int r, MessageTreeNode *parent = NULL);
    virtual ~MessageTreeNode();

    bool    Contains(int l, int r);
    bool    Contains(const MessageTreeNode * n);
    void    Insert(MessageTreeNode *node);
    bool    IsLeaf() const { return m_children.size() == 0; }

    bool    CheckValidity() const;
    void    Dump(File &f, const Message *msg, int level) const;
    void    DumpDot(File &f, const Message *msg) const;
    std::string GetDotName(const Message *msg) const;
    std::string GetDotStyle(const Message *msg) const;
    std::string GetDotLabel(const Message *msg) const;
    void    UpdateHistory(const MessageAccessLog *t);
    int     GetChildrenCount() const { return m_children.size(); }
    bool    HasFlag(NodeFlag f) const { return (m_flag & f) != 0; }
    void    SetSubMessage(Message *msg) { m_submsg = msg; }
    Message *   GetSubMessage() const { return m_submsg; }
private:
    void    AppendChild(MessageTreeNode *node);
    void    SetFlag(NodeFlag f) { m_flag |= f; }
private:
    int m_l, m_r;
    u32 m_flag;
    MessageTreeNode *m_parent;
    std::vector<MessageTreeNode *> m_children;
    ExecutionHistory    m_execHistory;
    Message *   m_submsg;
};

class MessageTree {
    friend class MessageTreeRefiner;
public:
    MessageTree(Message *msg);
    virtual ~MessageTree();

    void    Construct(const MessageAccessLog *log, MessageAccessComparator &cmp);
    void    Dump(File &f) const;
    void    DumpDot(File &f, bool isRoot) const;
    void    UpdateHistory(const MessageAccessLog *t);
    MessageTreeNode *   FindNode(const MemRegion &r);
    MessageTreeNode *   GetRoot() { return m_root; }
private:
    void    Insert(MessageTreeNode *node);
    bool    CheckValidity() const;
private:
    MessageTreeNode *m_root;
    Message *m_message;
    //const MessageAccessLog &m_log;
};

// class ParallelFieldDetector : public TraceAnalyzer {
// public:
//     ParallelFieldDetector(MessageTree &t);
//     virtual ~ParallelFieldDetector();
// 
//     void    Reset() override;
//     void    OnExecuteTrace(ExecuteTraceEvent &event) override;
//     void    OnComplete() override;
// 
// private:
//     MessageTree &m_tree;
// };

class MessageTreeRefiner {
public:
    MessageTreeRefiner();
    virtual ~MessageTreeRefiner();

    virtual void RefineTree(MessageTree &tree);
    virtual void Refine(MessageTreeNode *node);
    virtual void RefineNode(MessageTreeNode *node) = 0;
};

class TokenizeRefiner : public MessageTreeRefiner {
public:
    TokenizeRefiner(const Message *msg);

    virtual void RefineNode(MessageTreeNode *node) override;
private:
    bool IsTokenChar(byte ch) const;
    bool CanConcatenate(const MessageTreeNode *l, const MessageTreeNode *r) const;
private:
    const Message *m_msg;
};

class ParallelFieldDetector : public MessageTreeRefiner {
public:

    //virtual void Refine(MessageTree &tree);
    virtual void Refine(MessageTreeNode *node) override;
    virtual void RefineNode(MessageTreeNode *node) override;

private:
    bool    RefineOnce(MessageTreeNode *node);
    void    Reset();
    bool    CheckRefinableLeft(MessageTreeNode *node);
    bool    CheckRefinableRight(MessageTreeNode *node);
    bool    DoCheckRefinableRight(MessageTreeNode *node);
private:
    ExecutionHistory    m_commonExecHist;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_MSGTREE_H__