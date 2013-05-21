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
public:
    MessageTreeNode(int l, int r, MessageTreeNode *parent = NULL);
    virtual ~MessageTreeNode();

    bool    Contains(int l, int r);
    bool    Contains(const MessageTreeNode * n);
    void    Insert(MessageTreeNode *node);

    bool    CheckValidity() const;
    void    Dump(File &f, const Message *msg, int level) const;

private:
    int m_l, m_r;
    MessageTreeNode *m_parent;
    std::vector<MessageTreeNode *> m_children;
};

class MessageTree {
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

 
#endif // __PROPHET_PROTOCOL_ANALYZERS_MSGTREE_H__