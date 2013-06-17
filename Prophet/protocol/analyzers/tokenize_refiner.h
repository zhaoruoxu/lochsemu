#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__
#define __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__
 
#include "msgtree.h"


class TokenizeRefiner : public MessageTreeRefiner {
public:
    TokenizeRefiner(const Message *msg, MessageType type, int depth = 1);

    virtual void RefineNode(TreeNode *node) override;
    virtual void RefineTree(MsgTree &tree) override;
private:
    bool IsTokenChar(byte ch) const;
    bool CanConcatenate(const TreeNode *l, const TreeNode *r) const;
    void CalculateDepth(TreeNode *node);
private:
    const Message *m_msg;
    int m_depth;
    MessageType m_type;
    std::map<TreeNode *, int> m_nodeDepth;
};

 
#endif // __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__