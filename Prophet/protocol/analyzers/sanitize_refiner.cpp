#include "stdafx.h"
#include "sanitize_refiner.h"


SanitizeRefiner::SanitizeRefiner(  )
{
    
}

void SanitizeRefiner::Refine( MessageTreeNode *node )
{
    if (HasTrailingZero(node)) {
        MessageTreeNode *left = node->GetChild(0);
        MessageTreeNode *right = node->GetChild(1);
        Refine(left);
        node->m_flag = left->m_flag;
        for (auto &ch : left->m_children) {
            ch->m_parent = node;
        }
        node->m_children = left->m_children;
        node->m_children.push_back(right);
        left->m_children.clear();
        delete left;
    }
}

bool SanitizeRefiner::HasTrailingZero( MessageTreeNode *node )
{
    if (node->m_submsg != NULL) return false;
    if (node->GetChildrenCount() != 2) return false;
    MessageTreeNode *right = node->GetChild(1);
    return right->IsLeaf() && right->Length() == 1;
}
// 
// void SanitizeRefiner::ClearNode( MessageTreeNode *node ) const
// {
//     if (node->GetSubMessage() != NULL) {
//         for (auto &ch : node->m_children) {
//             SAFE_DELETE(ch);
//         }
//         node->m_children.clear();
//     } else {
//         for (auto &ch : node->m_children) {
//             ClearNode(ch);
//         }
//     }
// }
// 
// void SanitizeRefiner::RefineTree( MessageTree &tree )
// {
//     if (m_removeCrypto) {
//         ClearNode(tree.GetRoot());
//     }
//     MessageTreeRefiner::RefineTree(tree);
// }
// 
