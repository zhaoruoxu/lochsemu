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

void MessageTree::DumpDot( File &f ) const
{
    fprintf(f.Ptr(), "digraph MT {\n");

    m_root->DumpDot(f, m_message);

    fprintf(f.Ptr(), "}\n");
}

void MessageTree::UpdateHistory( const MessageAccessLog *t )
{
    m_root->UpdateHistory(t);
}

MessageTreeNode::MessageTreeNode( int l, int r, MessageTreeNode *parent )
    : m_l(l), m_r(r)
{
    Assert(m_l <= m_r);
    m_parent = parent;
    m_flag = 0;
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
    fprintf(f.Ptr(), "[%d,%d] %s  EH: ", m_l, m_r, GetMsgContent(msg).c_str());
    for (auto val : m_execHistory) {
        fprintf(f.Ptr(), "%08x ", val);
    }
    fprintf(f.Ptr(), "\n");

    for (auto c : m_children)
        c->Dump(f, msg, level+1);
}

void MessageTreeNode::DumpDot( File &f, const Message *msg ) const
{
    fprintf(f.Ptr(), "%s [shape=%s,label=\"%s\"%s];\n", GetDotNodeName().c_str(), 
        m_children.size() > 0 ? "box" : "ellipse", GetMsgContent(msg).c_str(),
        HasFlag(TREENODE_PARALLEL) ? ",color=blue" : "");
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
        byte ch = msg->Get(i).Data;
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

void MessageTreeNode::UpdateHistory( const MessageAccessLog *t )
{
    // do work
    for (int i = 0; i < t->Count(); i++) {
        const MessageAccess *ma = t->Get(i);
        if (ma->Offset == m_l) {
            m_execHistory.insert(GetProcStackHash(ma->CallStack));
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

void TokenizeRefiner::RefineNode( MessageTreeNode *node )
{
    if (node->IsLeaf()) return;

    std::vector<MessageTreeNode *> newChildren;
    newChildren.push_back(node->m_children[0]);
    MessageTreeNode *prev = node->m_children[0];
    for (uint i = 1; i < node->m_children.size(); i++) {
        if (CanConcatenate(prev, node->m_children[i])) {
            prev->m_r = node->m_children[i]->m_r;
            delete node->m_children[i];
        } else {
            prev = node->m_children[i];
            newChildren.push_back(prev);
        }
    }
    if (newChildren.size() == 1) {
        delete newChildren[0];
        node->m_children.clear();
    } else {
        node->m_children = newChildren;
    }
}

TokenizeRefiner::TokenizeRefiner( const Message *msg )
{
    m_msg = msg;
}

bool TokenizeRefiner::IsTokenChar( byte ch ) const
{
    return !isspace(ch) && !iscntrl(ch);
}

bool TokenizeRefiner::CanConcatenate(const MessageTreeNode *l, 
                                     const MessageTreeNode *r ) const
{
    return l->IsLeaf() && r->IsLeaf() && 
        IsTokenChar(m_msg->Get(l->m_r).Data) && 
        IsTokenChar(m_msg->Get(r->m_l).Data);
}

void ParallelFieldDetector::Refine( MessageTreeNode *node )
{
    RefineNode(node);
    for (auto &c : node->m_children)
        Refine(c);
}

void ParallelFieldDetector::RefineNode( MessageTreeNode *node )
{
    if (node->m_l == 12 && node->m_r == 57) {
        LxInfo("debug\n");
    }
    while (RefineOnce(node)) {
        Reset();
    }
}

void ParallelFieldDetector::Reset()
{
    m_commonExecHist.clear();
}

bool ParallelFieldDetector::CheckRefinableLeft( MessageTreeNode *node )
{
    //if (node->GetChildrenCount() != 2) return false;
    m_commonExecHist = node->m_execHistory;
    return true;
}

bool ParallelFieldDetector::CheckRefinableRight( MessageTreeNode *node )
{
    if (node->IsLeaf()) return false;
    return DoCheckRefinableRight(node);
}

bool ParallelFieldDetector::DoCheckRefinableRight( MessageTreeNode *node )
{
    if (node->m_execHistory != m_commonExecHist) return false;
    if (node->IsLeaf()) return true;
    if (node->m_children.size() != 2) return false;
    Assert(node->m_children[0]->m_execHistory == m_commonExecHist);
    return DoCheckRefinableRight(node->m_children[1]);
}

bool ParallelFieldDetector::RefineOnce( MessageTreeNode *node )
{
    for (int i = 0; i < node->GetChildrenCount() - 1; i++) {
        MessageTreeNode *left = node->m_children[i];
        MessageTreeNode *right = node->m_children[i+1];
        if (!CheckRefinableLeft(left)) continue;
        if (!CheckRefinableRight(right)) continue;

        LxInfo("Found parallel fields, refining\n");

        std::vector<MessageTreeNode *> newChildren;
        int p = 0;
        for (; p < i; p++) {
            newChildren.push_back(node->m_children[p]);
        }
        p = i + 2; // skip left and right
        MessageTreeNode *parallelNode = 
            new MessageTreeNode(left->m_l, right->m_r, node);
        parallelNode->AppendChild(left);
        while (true) {
            MessageTreeNode *r = right;
            if (r->IsLeaf()) {
                parallelNode->AppendChild(r);
                break;
            }
            Assert(r->GetChildrenCount() == 2);
            parallelNode->AppendChild(r->m_children[0]);
            right = r->m_children[1];
            r->m_children.clear();
            delete r;
        }
        newChildren.push_back(parallelNode);
        for (; p < node->GetChildrenCount(); p++)
            newChildren.push_back(node->m_children[p]);

        if (newChildren.size() == 1) {
            Assert(newChildren[0] == parallelNode);
            node->m_children = parallelNode->m_children;
            parallelNode->m_children.clear();
            delete parallelNode;
            node->SetFlag(TREENODE_PARALLEL);
        } else {
            node->m_children = newChildren;
            parallelNode->SetFlag(TREENODE_PARALLEL);
        }

        return true;
    }
    return false;
}
