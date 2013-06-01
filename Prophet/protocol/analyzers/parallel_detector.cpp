#include "stdafx.h"
#include "parallel_detector.h"


void ParallelFieldDetector::Refine( MessageTreeNode *node )
{
    RefineNode(node);
    for (auto &c : node->m_children)
        Refine(c);
}

void ParallelFieldDetector::RefineNode( MessageTreeNode *node )
{
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
    if (node->HasFlag(TREENODE_PARALLEL)) return false;
    m_commonExecHist = node->m_execHistory;
    return true;
}

bool ParallelFieldDetector::CheckRefinableRight( MessageTreeNode *node )
{
    if (node->HasFlag(TREENODE_PARALLEL)) return false;
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
    if (node->HasFlag(TREENODE_PARALLEL)) return false;
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
        parallelNode->m_execHistory = left->m_execHistory;
        parallelNode->m_execHistoryStrict = left->m_execHistoryStrict;
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
            parallelNode = node;
        } else {
            node->m_children = newChildren;
        }
        parallelNode->SetFlag(TREENODE_PARALLEL);
        LabelLeafFlags(parallelNode);
        return true;
    }
    return false;
}

void ParallelFieldDetector::LabelLeafFlags( MessageTreeNode *node )
{
    ExecHistory eh = node->m_execHistory;
    std::vector<MessageTreeNode *> possibleSeparators;
    std::deque<MessageTreeNode *> queue;
    std::set<MessageTreeNode *> parallelParents;
    queue.push_back(node);
    while (!queue.empty()) {
        MessageTreeNode *n = queue.front();
        queue.pop_front();
        if (n->IsLeaf()) {
            if (n->m_execHistory == eh && 
                parallelParents.find(n->m_parent) == parallelParents.end()) 
            {
                n->SetFlag(TREENODE_PARALLEL);
                parallelParents.insert(n->m_parent);
            } else {
                possibleSeparators.push_back(n);
            }
        } else {
            for (auto &ch : n->m_children) {
                queue.push_back(ch);
            }
        }
    }

    if (possibleSeparators.empty()) return;
    ExecHistory sep = possibleSeparators[0]->m_execHistory;
    for (uint i = 1; i < possibleSeparators.size(); i++)
        if (possibleSeparators[i]->m_execHistory != sep) return;
    for (auto &s : possibleSeparators) {
        s->SetFlag(TREENODE_SEPARATOR);
    }
}
