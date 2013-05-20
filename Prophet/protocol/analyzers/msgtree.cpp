#include "stdafx.h"
#include "msgtree.h"


bool StackHashComparator::Equals( const MessageAccess *l, const MessageAccess *r )
{
    Assert(l && r);
    return GetProcStackHash(l->CallStack) == GetProcStackHash(r->CallStack);
}


MessageTree::MessageTree( const MessageAccessLog &log )
    : m_log(log), m_root(NULL)
{

}

MessageTree::~MessageTree()
{
    SAFE_DELETE(m_root);
}

void MessageTree::Construct( MessageAccessComparator &cmp )
{
    LxDebug("Constructing message tree\n");
    const Message *msg = m_log.GetMessage();
    m_root = new MessageTreeNode(0, msg->Size() - 1);

    const MessageAccess *prev = m_log.Get(0);
    MessageTreeNode *currNode = new MessageTreeNode(prev->Offset, prev->Offset);
    for (int i = 1; i < m_log.Count(); i++) {
        const MessageAccess *curr = m_log.Get(i);

        if (curr->Offset == prev->Offset && cmp.Equals(curr, prev)) {
            // ignore local visits
            continue;
        }

        if (curr->Offset == prev->Offset+1 && cmp.Equals(curr, prev)) {
            currNode->m_r++;
        } else {
            LxInfo("inserting [%d,%d]\n", currNode->m_l, currNode->m_r);
            if (currNode->m_l == 28 && currNode->m_r == 102) {
                LxDebug("debug\n");
            }
            Insert(currNode);
            currNode = new MessageTreeNode(curr->Offset, curr->Offset);
        }
        prev = curr;
    }
    Insert(currNode);

#ifdef _DEBUG
    if (!CheckValidity()) {
        LxError("MessageTree validity check failed\n");
    }
#endif
}

void MessageTree::Insert( MessageTreeNode *node )
{
    MessageTreeNode *n = m_root;

    while (true) {
        Assert(n->Contains(node));
        MessageTreeNode *parent = n;
        for (auto c : n->m_children) {
            if (c->Contains(node)) {
                n = c;
                break;
            }
        }
        if (n == parent) {
            parent->Insert(node);
            break;
        }
    }
}

bool MessageTree::CheckValidity() const
{
    return m_root->CheckValidity();
}

void MessageTree::Dump( File &f ) const
{
    m_root->Dump(f, m_log.GetMessage(), 0);
}

MessageTreeNode::MessageTreeNode( int l, int r )
    : m_l(l), m_r(r)
{
    Assert(m_l <= m_r);
    m_parent = NULL;
}

MessageTreeNode::~MessageTreeNode()
{
    for (auto &node : m_children) {
        SAFE_DELETE(node);
    }
    m_children.clear();
}

bool MessageTreeNode::Contains( int l, int r )
{
    return m_l <= l && m_r >= r;
}

bool MessageTreeNode::Contains( const MessageTreeNode * n )
{
    return Contains(n->m_l, n->m_r);
}

void MessageTreeNode::Insert( MessageTreeNode *node )
{
    Assert(Contains(node));
    Assert(node->m_children.size() == 0);

    if (m_l == node->m_l && m_r == node->m_r) {
        delete node;
        return;
    }

    node->m_parent = this;

    if (m_children.size() == 0) {
        // leaf node
        if (m_l < node->m_l) {
            MessageTreeNode *n = new MessageTreeNode(m_l, node->m_l-1);
            n->m_parent = this;
            m_children.push_back(n);
        }
        m_children.push_back(node);
        if (m_r > node->m_r) {
            MessageTreeNode *n = new MessageTreeNode(node->m_r+1, m_r);
            n->m_parent = this;
            m_children.push_back(n);
        }
        return;
    }

    std::vector<MessageTreeNode *> newChildren;
    
    newChildren.push_back(node);
    for (auto n : m_children) {
        if (n->m_r < node->m_l || n->m_l > node->m_r) {
            // no overlap
            newChildren.push_back(n);
        } else if (n->m_l < node->m_l && n->m_r >= node->m_l) {
            // n left-overlaps with node
            LxWarning("MessageTreeNode left-overlap\n");
            MessageTreeNode *newChild = new MessageTreeNode(node->m_l, n->m_r);
            // node->m_l = newChild->m_r + 1;
            n->m_r = newChild->m_l - 1;
            newChild->m_parent = node;
            node->m_children.push_back(newChild);
            newChildren.push_back(n);
        } else if (n->m_l <= node->m_r && n->m_r > node->m_r) {
            // r right-overlaps with node
            LxWarning("MessageTreeNode right-overlap\n");
            MessageTreeNode *newChild = new MessageTreeNode(n->m_l, node->m_r);
            // node->m_r = newChild->m_l - 1;
            n->m_l = newChild->m_r + 1;
            newChild->m_parent = node;
            node->m_children.push_back(newChild);
            newChildren.push_back(n);
        } else {
            Assert(node->Contains(n));
            n->m_parent = node;
            node->m_children.push_back(n);
        }
    }
    m_children = newChildren;
    std::sort(m_children.begin(), m_children.end(), 
        [](const MessageTreeNode *l, const MessageTreeNode *r) -> bool
    {
        return l->m_l < r->m_l;
    });
}

bool MessageTreeNode::CheckValidity() const
{
    if (m_children.size() == 0)
        return m_l <= m_r;    // leaf node

    int l = m_l-1;
    for (auto c : m_children) {
        if (c->m_l != l+1) return false;
        l = c->m_r;
        if (!c->CheckValidity()) return false;
    }
    if (l != m_r) return false;
    return true;
}

void MessageTreeNode::Dump( File &f, const Message *msg, int level ) const
{
    for (int i = 0; i < level; i++)
        fprintf(f.Ptr(), "\t");
    fprintf(f.Ptr(), "[%d,%d] '", m_l, m_r);
    for (int i = m_l; i <= m_r; i++) {
        char ch = (*msg)[i].Data;
        fprintf(f.Ptr(), "%c", isprint(ch) ? ch : '.');
    }
    fprintf(f.Ptr(), "'\n");

    for (auto c : m_children)
        c->Dump(f, msg, level+1);

}
