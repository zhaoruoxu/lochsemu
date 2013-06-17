#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__
#define __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__
 
#include "protocol/analyzer.h"

class DirectionField : public TraceAnalyzer {
public:
    DirectionField(Message *msg, TaintEngine *taint);
    virtual ~DirectionField();

    void Reset() override;
    void OnExecuteTrace(ExecuteTraceEvent &event) override;
    void OnComplete() override;

private:
    void Analyze(const TaintRegion &tr, TreeNode *node);
private:
    TaintEngine *m_taint;
    Message *m_message;
    TreeNode *m_currentNode;
    std::map<TreeNode *, TreeNode *> m_discovered;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__