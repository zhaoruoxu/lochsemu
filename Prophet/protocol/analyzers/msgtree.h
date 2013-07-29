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

typedef std::set<u32>       ExecHistory;
typedef std::vector<u32>    ExecHistoryStrict;

enum NodeFlag {
    TREENODE_PARALLEL   = 1 << 0,
    TREENODE_SEPARATOR  = 1 << 1,
};

class TreeNode;
class Message;

struct NodeLink {
    TreeNode *Target;
    Message *Msg;
    std::string Desc;
    std::string Dir;

    NodeLink(TreeNode *target, Message *msg, const std::string &desc,
        const std::string &dir)
        : Target(target), Msg(msg), Desc(desc), Dir(dir)
    {
        Assert(dir == "forward" || dir == "back" || dir == "both" || dir == "none");
    }
};

class TreeNode {
    friend class MsgTree;
    friend class MessageTreeRefiner;
    friend class TokenizeRefiner;
    friend class ParallelFieldDetector;
    friend class SanitizeRefiner;
public:
    TreeNode(int l, int r, TreeNode *parent = NULL);
    virtual ~TreeNode();

    bool    Contains(int l, int r);
    bool    Contains(const TreeNode * n);
    void    Insert(TreeNode * &node);
    bool    IsLeaf() const { return m_children.size() == 0; }
    int     Length() const { return m_r - m_l + 1; }
    int     L() const { return m_l; }
    int     R() const { return m_r; }

    bool    CheckValidity() const;
    void    Dump(File &f, const Message *msg, int level) const;
    void    DumpDot(File &f, const Message *msg) const;
    std::string GetDotName(const Message *msg) const;
    std::string GetDotStyle(const Message *msg) const;
    std::string GetDotLabel(const Message *msg) const;
    void    UpdateHistory(const MessageAccessLog *t);
    int     GetChildrenCount() const { return m_children.size(); }
    bool    HasFlag(NodeFlag f) const { return (m_flag & f) != 0; }
    //void    SetSubMessage(Message *msg) { Assert(!m_submsg); m_submsg = msg; }
    void    AddSubMessage(Message *msg) { m_subMessages.push_back(msg); }
    //Message *   GetSubMessage() const { return m_submsg; }
    TreeNode *GetChild(int n) const;
    TreeNode *GetParent() const { return m_parent; }
    void    ClearChildren();
    void    AddLink(const NodeLink &link) { 
        m_links.push_back(link);
    }
private:
    void    AppendChild(TreeNode *node);
    void    SetFlag(NodeFlag f) { m_flag |= f; }
    bool    HasSubMessage() const;
    void    DoClearChildren();
    void    FixParent();
private:
    int m_l, m_r;
    u32 m_flag;
    TreeNode *m_parent;
    std::vector<TreeNode *> m_children;
    ExecHistory     m_execHistory;
    ExecHistoryStrict   m_execHistoryStrict;
    std::vector<Message *>  m_subMessages;
    std::vector<NodeLink> m_links;
};

class MsgTree {
    friend class MessageTreeRefiner;
public:
    MsgTree(Message *msg);
    virtual ~MsgTree();

    void    Construct(const MessageAccessLog *log, MessageAccessComparator &cmp);
    void    Dump(File &f) const;
    void    DumpDot(File &f, bool isRoot) const;
    void    UpdateHistory(const MessageAccessLog *t);
    TreeNode *   FindOrCreateNode(const MemRegion &r);
    TreeNode *   FindOrCreateNode(const TaintRegion &tr);
    TreeNode *   GetRoot() { return m_root; }
    TreeNode *   FindNode(u32 addr) const;
    TreeNode *   FindNode(const TaintRegion &r) const;

private:
    void    Insert(TreeNode *node);
    bool    CheckValidity() const;
    TreeNode *   DoFindNode(const MemRegion &r) const;
    TreeNode *   DoFindNode(const TaintRegion &tr) const;
    
private:
    TreeNode *m_root;
    Message *m_message;
    //const MessageAccessLog &m_log;
};

class MessageTreeRefiner {
public:
    MessageTreeRefiner();
    virtual ~MessageTreeRefiner();

    virtual void RefineTree(MsgTree &tree);
    virtual void Refine(TreeNode *node);
    virtual void RefineNode(TreeNode *node) {};
};

#endif // __PROPHET_PROTOCOL_ANALYZERS_MSGTREE_H__