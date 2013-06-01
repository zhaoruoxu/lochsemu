#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_SANITIZE_REFINER_H__
#define __PROPHET_PROTOCOL_ANALYZERS_SANITIZE_REFINER_H__
 
#include "msgtree.h"

class SanitizeRefiner : public MessageTreeRefiner {
public:
    virtual void Refine(MessageTreeNode *node) override;

private:
    bool    HasTrailingZero(MessageTreeNode *node);
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_SANITIZE_REFINER_H__