#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_FIELDFORMAT_H__
#define __PROPHET_PROTOCOL_ANALYZERS_FIELDFORMAT_H__
 
#include "protocol/analyzer.h"

/*
struct TokenInfo {
    bool HasTrueCompare;
    bool HasFalseCompare;

    TokenInfo() {
        HasTrueCompare = HasFalseCompare = false;
    }
};

class MessageFieldFormat : public TraceAnalyzer {
public:
    MessageFieldFormat(TaintEngine *taint);
    virtual ~MessageFieldFormat();

    virtual void Reset() override;
    virtual void OnExecuteTrace(ExecuteTraceEvent &event) override;
    virtual void OnComplete() override;

private:
    TaintEngine *   m_taint;
    TokenInfo       m_tokens[256];
};
*/
#endif // __PROPHET_PROTOCOL_ANALYZERS_FIELDFORMAT_H__