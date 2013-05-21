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

class MessageTreeNode {
    friend class MessageTree;
    friend class MessageTreeRefiner;
    friend class TokenizeRefiner;
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
    std::string GetDotNodeName() const;
    std::string GetMsgContent(const Message *msg) const;

private:
    int m_l, m_r;
    MessageTreeNode *m_parent;
    std::vector<MessageTreeNode *> m_children;
};

class MessageTree {
    friend class MessageTreeRefiner;
public:
    MessageTree(const MessageAccessLog &log);
    virtual ~MessageTree();

    void    Construct(MessageAccessComparator &cmp);
    void    Dump(File &f) const;
    void    DumpDot(File &f) const;
private:
    void    Insert(MessageTreeNode *node);
    bool    CheckValidity() const;
private:
    MessageTreeNode *m_root;
    const MessageAccessLog &m_log;
};

class MessageTreeRefiner {
public:
    MessageTreeRefiner();
    virtual ~MessageTreeRefiner();

    virtual void Refine(MessageTree &tree);
    virtual void Refine(MessageTreeNode *node);
    virtual void RefineNode(MessageTreeNode *node) = 0;
};

class TokenizeRefiner : public MessageTreeRefiner {
public:
    TokenizeRefiner(const Message *msg);

    virtual void RefineNode(MessageTreeNode *node) override;
private:
    bool IsTokenChar(char ch) const;
    bool CanConcatenate(const MessageTreeNode *l, const MessageTreeNode *r) const;
private:
    const Message *m_msg;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_MSGTREE_H__