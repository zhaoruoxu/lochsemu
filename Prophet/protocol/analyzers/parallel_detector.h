#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_PARALLEL_DETECTOR_H__
#define __PROPHET_PROTOCOL_ANALYZERS_PARALLEL_DETECTOR_H__
 
#include "msgtree.h"

class ParallelFieldDetector : public MessageTreeRefiner {
public:
    virtual void Refine(MessageTreeNode *node) override;
    virtual void RefineNode(MessageTreeNode *node) override;

private:
    bool    RefineOnce(MessageTreeNode *node);
    void    Reset();
    bool    CheckRefinableLeft(MessageTreeNode *node);
    bool    CheckRefinableRight(MessageTreeNode *node);
    bool    DoCheckRefinableRight(MessageTreeNode *node, int &hitcount);

private:
    void    ToList(MessageTreeNode *&node, std::vector<MessageTreeNode *> &list);
    void    LabelNode(MessageTreeNode *node);
private:
    ExecHistory     m_parallelExecHist;
    ExecHistory     m_separatorExecHist;
    bool            m_foundSeparator;
};

 
#endif // __PROPHET_PROTOCOL_ANALYZERS_PARALLEL_DETECTOR_H__