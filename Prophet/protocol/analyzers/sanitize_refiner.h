#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_SANITIZE_REFINER_H__
#define __PROPHET_PROTOCOL_ANALYZERS_SANITIZE_REFINER_H__
 
#include "msgtree.h"

class SanitizeRefiner : public MessageTreeRefiner {
public:
    SanitizeRefiner();
    //virtual void RefineTree(MessageTree &tree) override;
    virtual void Refine(MessageTreeNode *node) override;

private:
    //void    ClearNode(MessageTreeNode *node) const;
private:
    bool    HasTrailingZero(MessageTreeNode *node);
    //bool    m_removeCrypto;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_SANITIZE_REFINER_H__