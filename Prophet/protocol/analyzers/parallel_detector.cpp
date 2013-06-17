#include "stdafx.h"
#include "parallel_detector.h"


void ParallelFieldDetector::Refine( TreeNode *node )
{
    RefineNode(node);
    for (auto &c : node->m_children)
        Refine(c);
}

void ParallelFieldDetector::RefineNode( TreeNode *node )
{
    while (RefineOnce(node)) {
    }
}

void ParallelFieldDetector::Reset()
{
    m_parallelExecHist.clear();
    m_separatorExecHist.clear();
    m_foundSeparator = false;
}

bool ParallelFieldDetector::CheckRefinableLeft( TreeNode *node )
{
    Reset();
    if (!node->IsLeaf()) return false;
    if (node->HasFlag(TREENODE_PARALLEL)) return false;
    m_parallelExecHist = node->m_execHistory;
    return true;
}

bool ParallelFieldDetector::CheckRefinableRight( TreeNode *node )
{
    if (node->HasFlag(TREENODE_PARALLEL)) return false;
    if (node->IsLeaf()) return false;
    int hitcount = 0;
    bool r = DoCheckRefinableRight(node, hitcount);
    return r && hitcount > 1;
}

bool ParallelFieldDetector::DoCheckRefinableRight( TreeNode *node, int &hitcount )
{
    if (node->IsLeaf()) {
        if (node->m_execHistory == m_parallelExecHist) {
            hitcount++;
            return true;
        }
        if (!m_foundSeparator) {
            m_separatorExecHist = node->m_execHistory;
            m_foundSeparator = true;
            return true;
        }
        if (m_foundSeparator && node->m_execHistory == m_separatorExecHist)
            return true;
        return false;
    }
    int parallelHitCount = hitcount;
    for (auto &ch : node->m_children) {
        if (!DoCheckRefinableRight(ch, hitcount)) return false;
        parallelHitCount = hitcount - parallelHitCount;
    }
    return parallelHitCount > 0;
}


bool ParallelFieldDetector::RefineOnce( TreeNode *node )
{
    if (node->HasFlag(TREENODE_PARALLEL)) return false;
    for (int i = 0; i < node->GetChildrenCount() - 1; i++) {
        TreeNode *left = node->m_children[i];
        TreeNode *right = node->m_children[i+1];
        if (!CheckRefinableLeft(left)) continue;
        if (!CheckRefinableRight(right)) continue;

        LxInfo("Found parallel fields, refining\n");

        std::vector<TreeNode *> newChildren;
        int p = 0;
        for (; p < i; p++) {
            newChildren.push_back(node->m_children[p]);
        }
        p = i + 2; // skip left and right
        TreeNode *parallelNode = 
            new TreeNode(left->m_l, right->m_r, node);
        parallelNode->AppendChild(left);
        parallelNode->m_execHistory = left->m_execHistory;
        parallelNode->m_execHistoryStrict = left->m_execHistoryStrict;
        ToList(right, parallelNode->m_children);

        newChildren.push_back(parallelNode);
        for (; p < node->GetChildrenCount(); p++)
            newChildren.push_back(node->m_children[p]);

        if (newChildren.size() == 1) {
            Assert(newChildren[0] == parallelNode);
            node->m_children = parallelNode->m_children;
            parallelNode->m_children.clear();
            delete parallelNode;
            parallelNode = node;
        } else {
            node->m_children = newChildren;
        }
        parallelNode->SetFlag(TREENODE_PARALLEL);
        for (auto &c : parallelNode->m_children) {
            LabelNode(c);
        }
        return true;
    }
    return false;
}

void ParallelFieldDetector::ToList( TreeNode *&node, std::vector<TreeNode *> &list )
{
    if (node->IsLeaf()) {
        list.push_back(node);
        return;
    }
    for (auto &ch : node->m_children) {
        ToList(ch, list);
    }
    node->m_children.clear();
    SAFE_DELETE(node);
}

void ParallelFieldDetector::LabelNode( TreeNode *node )
{
    if (node->m_execHistory == m_parallelExecHist) {
        node->SetFlag(TREENODE_PARALLEL);
    } else if (node->m_execHistory == m_separatorExecHist) {
        node->SetFlag(TREENODE_SEPARATOR);
    } else {
        LxFatal("shit\n");
    }
}
