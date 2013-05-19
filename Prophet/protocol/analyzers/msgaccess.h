#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_MSGACCESS_H__
#define __PROPHET_PROTOCOL_ANALYZERS_MSGACCESS_H__
 
#include "protocol/analyzer.h"
#include "protocol/message.h"
#include "callstack.h"

struct MessageAccess {
    int Offset;
    const TContext *Context;
    ProcStack CallStack;
};

class MessageAccessLog : public TraceAnalyzer {
public:
    MessageAccessLog(CallStack *cs, const Message *msg);
    virtual ~MessageAccessLog();

    void Reset() override;
    void OnExecuteTrace(ExecuteTraceEvent &event) override;
    void OnComplete() override;
    void Dump(File &f) const;
private:
    void OnMemRead(const TContext *t, u32 addr, byte data);
private:
    CallStack *     m_callstack;
    const Message * m_currmsg;
    std::vector<MessageAccess *>    m_accesses;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_MSGACCESS_H__