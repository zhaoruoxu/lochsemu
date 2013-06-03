#include "stdafx.h"
#include "msgtree.h"


bool StackHashComparator::Equals( const MessageAccess *l, const MessageAccess *r )
{
    Assert(l && r);
    return GetProcStackHash(l->CallStack) == GetProcStackHash(r->CallStack);
}


MessageTree::MessageTree(Message *msg)
    : m_root(NULL), m_message(msg)
{

}

MessageTree::~MessageTree()
{
    SAFE_DELETE(m_root);
}

void MessageTree::Construct( const MessageAccessLog *log, MessageAccessComparator &cmp )
{
    LxDebug("Constructing message tree\n");
    m_root = new MessageTreeNode(0, m_message->Size() - 1);
    //m_root->m_children.push_back(new MessageTreeNode(0, msg->Size() - 1, m_root));

    const MessageAccess *prev = log->Get(0);
    MessageTreeNode *currNode = new MessageTreeNode(prev->Offset, prev->Offset);
    for (int i = 1; i < log->Count(); i++) {
        const MessageAccess *curr = log->Get(i);

        if (curr->Offset == prev->Offset && cmp.Equals(curr, prev)) {
            // ignore local visits
            continue;
        }

        if (curr->Offset == prev->Offset+1 && cmp.Equals(curr, prev)) {
            currNode->m_r++;
        } else {
            //LxInfo("inserting [%d,%d]\n", currNode->m_l, currNode->m_r);
            //if (currNode->m_l == 60 && currNode->m_r == 66) {
            //    LxDebug("debug\n");
            //}
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
    m_root->Dump(f, m_message, 0);
}

void MessageTree::DumpDot( File &f, bool isRoot ) const
{
    if (isRoot) {
        fprintf(f.Ptr(), "digraph %s {\n", m_message->GetName().c_str());
        fprintf(f.Ptr(), "node [fontname=\"Consolas\", fontsize=14];\n");
    } else {
        fprintf(f.Ptr(), "subgraph cluster_%s {\n", m_message->GetName().c_str());
    }

    m_root->DumpDot(f, m_message);

    fprintf(f.Ptr(), "}\n");
}

void MessageTree::UpdateHistory( const MessageAccessLog *t )
{
    m_root->UpdateHistory(t);
}

MessageTreeNode * MessageTree::FindNode( const MemRegion &r )
{
    Assert(m_message->GetRegion().Contains(r));

    int left = r.Addr - m_message->GetRegion().Addr;
    int right = left + r.Len - 1;
    MessageTreeNode *n = m_root;
    while (true) {
        Assert(n->m_l <= left && n->m_r >= right);
        if (n->m_l == left && n->m_r == right) return n;
        if (n->IsLeaf()) {
            MessageTreeNode *newNode = new MessageTreeNode(left, right, n);
            n->Insert(newNode);
            return newNode;
        }

        bool found = false;
        for (MessageTreeNode *ch : n->m_children) {
            if (ch->m_l <= left && ch->m_r >= right) {
                n = ch;
                found =  true;
                break;
            }
        }
        if (!found) { return NULL; }
    }
}

MessageTreeNode::MessageTreeNode( int l, int r, MessageTreeNode *parent )
    : m_l(l), m_r(r)
{
    Assert(m_l <= m_r);
    m_parent = parent;
    m_flag = 0;
    m_submsg = NULL;
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
    } else {
        delete node;
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
    fprintf(f.Ptr(), "[%d,%d] %s  EH:(", m_l, m_r, GetDotLabel(msg).c_str());
    for (auto val : m_execHistory) {
        fprintf(f.Ptr(), "%08x ", val);
    }
    fprintf(f.Ptr(), "), SEH:(");
    for (auto val : m_execHistoryStrict) {
        fprintf(f.Ptr(), "%08x ", val);
    }
    fprintf(f.Ptr(), ")\n");

    for (auto c : m_children)
        c->Dump(f, msg, level+1);
}

void MessageTreeNode::DumpDot( File &f, const Message *msg ) const
{
    fprintf(f.Ptr(), "%s [label=\"%s\",%s];\n", GetDotName(msg).c_str(), 
        GetDotLabel(msg).c_str(), GetDotStyle(msg).c_str());
    std::string nodeName = GetDotName(msg);
    for (auto &c : m_children) {
        c->DumpDot(f, msg);
        fprintf(f.Ptr(), "%s -> %s;\n", nodeName.c_str(), c->GetDotName(msg).c_str());
    }

    if (m_submsg != NULL) {
        std::string tagName = m_submsg->GetName() + "_tag";
        fprintf(f.Ptr(), "%s [shape=box,color=blueviolet,style=filled,fontcolor=white,label=", tagName.c_str());
        m_submsg->GetTag()->DumpDot(f);
        fprintf(f.Ptr(), "];\n");
        fprintf(f.Ptr(), "%s -> %s;\n", nodeName.c_str(), tagName.c_str());
        m_submsg->GetTree()->DumpDot(f, false);
        fprintf(f.Ptr(), "%s -> %s;\n", tagName.c_str(), 
            m_submsg->GetTree()->GetRoot()->GetDotName(m_submsg).c_str());
    }
}

std::string MessageTreeNode::GetDotName(const Message *msg) const
{
    std::stringstream ss;
    ss << msg->GetName() << "__" << m_l << "_" << m_r;
    return ss.str();
}

std::string MessageTreeNode::GetDotStyle( const Message *msg ) const
{
    std::stringstream ss;
    if (IsLeaf()) {
        //ss << "shape=ellipse";
        if (HasFlag(TREENODE_PARALLEL)) {
            ss << "shape=ellipse,color=blue";
        } else if (HasFlag(TREENODE_SEPARATOR)) {
            ss << "shape=diamond,color=darkgreen";
        }
    } else {
        ss << "shape=box";
        if (HasFlag(TREENODE_PARALLEL)) {
            ss << ",color=blue,style=bold";
        } 
    }
    return ss.str();
}

std::string MessageTreeNode::GetDotLabel( const Message *msg ) const
{
    char buf[64];
    sprintf(buf, "%s[%x:%d]", this == msg->GetTree()->GetRoot() ? "Root " : "",
        msg->GetRegion().Addr + m_l, m_r - m_l + 1);
    //if (IsLeaf()) {
    strcat(buf, "\\n");
    return buf + ByteArrayToDotString(msg->GetRaw() + m_l, m_r - m_l + 1, 
        IsLeaf() ? 12 : 8);
    //} else {
    //    return buf;
    //}
}

void MessageTreeNode::UpdateHistory( const MessageAccessLog *t )
{
    // do work
    for (int i = 0; i < t->Count(); i++) {
        const MessageAccess *ma = t->Get(i);
        if (ma->Offset == m_l) {
            u32 entry = GetProcStackHash(ma->CallStack);
            m_execHistory.insert(entry);
            m_execHistoryStrict.push_back(entry);
        }
    }

    for (auto &c : m_children) {
        c->UpdateHistory(t);
    }
}

void MessageTreeNode::AppendChild( MessageTreeNode *node )
{
    node->m_parent = this;
    m_children.push_back(node);
}

MessageTreeNode * MessageTreeNode::GetChild( int n ) const
{
    Assert(n >= 0 && n < GetChildrenCount());
    return m_children[n];
}

void MessageTreeNode::ClearChildren()
{
    for (auto &ch : m_children) {
        SAFE_DELETE(ch);
    }
    m_children.clear();
}

MessageTreeRefiner::MessageTreeRefiner()
{

}

MessageTreeRefiner::~MessageTreeRefiner()
{

}

void MessageTreeRefiner::RefineTree( MessageTree &tree )
{
    Refine(tree.m_root);
    if (!tree.CheckValidity()) {
        LxFatal("Tree validity failed after refining\n");
    }
}

void MessageTreeRefiner::Refine(MessageTreeNode *node)
{
    for (auto &c : node->m_children)
        Refine(c);
    RefineNode(node);
}
