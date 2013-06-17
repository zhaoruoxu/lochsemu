#include "stdafx.h"
#include "sanitize_refiner.h"


SanitizeRefiner::SanitizeRefiner(  )
{
    
}

void SanitizeRefiner::Refine( TreeNode *node )
{
    if (HasTrailingZero(node)) {
        TreeNode *left = node->GetChild(0);
        TreeNode *right = node->GetChild(1);
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

bool SanitizeRefiner::HasTrailingZero( TreeNode *node )
{
    if (node->m_subMessages.size() != 0) return false;
    if (node->GetChildrenCount() != 2) return false;
    TreeNode *right = node->GetChild(1);
    return right->IsLeaf() && right->Length() == 1;
}

