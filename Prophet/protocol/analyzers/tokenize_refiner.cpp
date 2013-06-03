#include "stdafx.h"
#include "tokenize_refiner.h"

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
//     return l->IsLeaf() && r->IsLeaf() && 
//         IsTokenChar(m_msg->Get(l->m_r)) && 
//         IsTokenChar(m_msg->Get(r->m_l));
    if (!l->IsLeaf() || !r->IsLeaf()) return false;
    for (int i = l->m_l; i <= l->m_r; i++)
        if (!IsTokenChar(m_msg->Get(i))) return false;
    for (int i = r->m_l; i <= r->m_r; i++)
        if (!IsTokenChar(m_msg->Get(i))) return false;
    return true;
}
