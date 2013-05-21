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
    //m_root->m_children.push_back(new MessageTreeNode(0, msg->Size() - 1, m_root));

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
            if (currNode->m_l == 60 && currNode->m_r == 66) {
                LxDebug("debug\n");
            }
            Insert(currNode);

//             if (!CheckValidity()) {
//                 LxError("MessageTree validity check failed\n");
//             }

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
    m_root->Insert(node);
}

bool MessageTree::CheckValidity() const
{
    return m_root->CheckValidity();
}

void MessageTree::Dump( File &f ) const
{
    m_root->Dump(f, m_log.GetMessage(), 0);
}

void MessageTree::DumpDot( File &f ) const
{
    fprintf(f.Ptr(), "digraph MT {\n");

    m_root->DumpDot(f, m_log.GetMessage());

    fprintf(f.Ptr(), "}\n");
}

MessageTreeNode::MessageTreeNode( int l, int r, MessageTreeNode *parent )
    : m_l(l), m_r(r)
{
    Assert(m_l <= m_r);
    m_parent = parent;
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

    if (m_children.size() == 0) {
        // leaf node, extend to non-leaf
        if (m_l < node->m_l) {
            m_children.push_back(new MessageTreeNode(m_l, node->m_l-1, this));
        }
        node->m_parent = this;
        m_children.push_back(node);
        if (m_r > node->m_r) {
            m_children.push_back(new MessageTreeNode(node->m_r+1, m_r, this));
        }
        return;
    }

    for (auto c : m_children) {
        if (c->Contains(node)) {
            c->Insert(node);        // found a child to insert into
            return;
        }
    }

    // no big child, so insert to this
    node->m_parent = this;
    std::vector<MessageTreeNode *> newChildren;
    for (auto c : m_children) {
        if (c->m_r < node->m_l || c->m_l > node->m_r) {
            newChildren.push_back(c);
            continue;
        }
        if (c->m_l < node->m_l) {
            // left-overlap
            c->Insert(new MessageTreeNode(node->m_l, c->m_r));
            newChildren.push_back(c);
            node->m_l = c->m_r+1;
        } else if (c->m_r > node->m_r) {
            // right-overlap
            c->Insert(new MessageTreeNode(c->m_l, node->m_r));
            newChildren.push_back(c);
            node->m_r = c->m_l-1;
        } else {
            c->m_parent = node;
            node->m_children.push_back(c);
        }
    }
    if (node->m_l <= node->m_r) {
        if (node->m_children.size() == 1) {
            Assert(node->m_l == node->m_children[0]->m_l && 
                node->m_r == node->m_children[0]->m_r);
            delete node->m_children[0];
            node->m_children.clear();
        }
        newChildren.push_back(node);
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

    if (m_children.size() == 1)
        return false;       // then equals parent

    int l = m_l-1;
    for (auto c : m_children) {
        if (c->m_l != l+1) 
            return false;
        l = c->m_r;
        if (!c->CheckValidity()) 
            return false;
    }
    if (l != m_r) 
        return false;
    return true;
}

void MessageTreeNode::Dump( File &f, const Message *msg, int level ) const
{
    for (int i = 0; i < level; i++)
        fprintf(f.Ptr(), " ");
    fprintf(f.Ptr(), "[%d,%d] %s\n", m_l, m_r, GetMsgContent(msg).c_str());

    for (auto c : m_children)
        c->Dump(f, msg, level+1);
}

void MessageTreeNode::DumpDot( File &f, const Message *msg ) const
{
    fprintf(f.Ptr(), "%s [shape=%s,label=\"%s\"];\n", GetDotNodeName().c_str(), 
        m_children.size() > 0 ? "box" : "ellipse", GetMsgContent(msg).c_str());
    for (auto &c : m_children) {
        c->DumpDot(f, msg);
        fprintf(f.Ptr(), "%s -> %s;\n", GetDotNodeName().c_str(), c->GetDotNodeName().c_str());
    }
}

std::string MessageTreeNode::GetDotNodeName() const
{
    std::stringstream ss;
    ss << "node_" << m_l << "_" << m_r;
    return ss.str();
}

std::string MessageTreeNode::GetMsgContent( const Message *msg ) const
{
    std::stringstream ss;
    ss << "'";
    for (int i = m_l; i <= m_r; i++) {
        char ch = (*msg)[i].Data;
        if (isprint(ch))
            ss << ch;
        else {
            char buf[16];
            sprintf(buf, "\\\\0x%02x", ch);
            ss << buf;
        }
    }
    ss << "'";
    return ss.str();
}
