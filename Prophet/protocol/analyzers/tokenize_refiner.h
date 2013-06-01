#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__
#define __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__
 
#include "msgtree.h"


class TokenizeRefiner : public MessageTreeRefiner {
public:
    TokenizeRefiner(const Message *msg);

    virtual void RefineNode(MessageTreeNode *node) override;
private:
    bool IsTokenChar(byte ch) const;
    bool CanConcatenate(const MessageTreeNode *l, const MessageTreeNode *r) const;
private:
    const Message *m_msg;
};

 
#endif // __PROPHET_PROTOCOL_ANALYZERS_TOKENIZE_REFINER_H__