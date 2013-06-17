#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_PARALLEL_DETECTOR_H__
#define __PROPHET_PROTOCOL_ANALYZERS_PARALLEL_DETECTOR_H__
 
#include "msgtree.h"

class ParallelFieldDetector : public MessageTreeRefiner {
public:
    ParallelFieldDetector(int minlen);
    virtual void Refine(TreeNode *node) override;
    virtual void RefineNode(TreeNode *node) override;

private:
    bool    RefineOnce(TreeNode *node);
    void    Reset();
    bool    CheckRefinableLeft(TreeNode *node);
    bool    CheckRefinableRight(TreeNode *node);
    bool    DoCheckRefinableRight(TreeNode *node, int &hitcount);

private:
    void    ToList(TreeNode *&node, std::vector<TreeNode *> &list);
    void    LabelNode(TreeNode *node);
private:
    ExecHistory     m_parallelExecHist;
    ExecHistory     m_separatorExecHist;
    bool            m_foundSeparator;
    int             m_minlen;
};

 
#endif // __PROPHET_PROTOCOL_ANALYZERS_PARALLEL_DETECTOR_H__