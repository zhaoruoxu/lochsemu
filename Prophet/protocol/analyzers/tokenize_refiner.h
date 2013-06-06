#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__
#define __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__
 
#include "msgtree.h"


class TokenizeRefiner : public MessageTreeRefiner {
public:
    TokenizeRefiner(const Message *msg, int depth = 1);

    virtual void RefineNode(MessageTreeNode *node) override;
    virtual void RefineTree(MessageTree &tree) override;
private:
    bool IsTokenChar(byte ch) const;
    bool CanConcatenate(const MessageTreeNode *l, const MessageTreeNode *r) const;
    void CalculateDepth(MessageTreeNode *node);
private:
    const Message *m_msg;
    int m_depth;
    std::map<MessageTreeNode *, int> m_nodeDepth;
};

 
#endif // __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__