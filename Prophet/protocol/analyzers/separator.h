#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_SEPARATOR_H__
#define __PROPHET_PROTOCOL_ANALYZERS_SEPARATOR_H__
 
#include "protocol/analyzer.h"
#include "protocol/taint/taint.h"

/*
class SeparatorKeyword : public ProtocolAnalyzer {
public:
    SeparatorKeyword(Protocol *protocol);

    void    Initialize() override;
    void    OnPreExecute(PreExecuteEvent &event) override;
    void    OnPostExecute(PostExecuteEvent &event) override;
    void    OnMessageBegin(MessageBeginEvent &event) override;
    void    OnMessageEnd(MessageEndEvent &event) override;

    void    Serialize(Json::Value &root) const override;
    void    Deserialize(Json::Value &root) override;
private:
    void    Reset();
    void    CheckToken(byte t);

private:
    //TaintEngine *       m_taint;
    MessageManager *    m_msgmgr;
    int     m_msglen;

    Taint   m_tokenTable[256];
    bool    m_hasCmpSuccessed[256];
    bool    m_hasCmpFailed[256];

    int     m_tokenMinCmpRange;
};

 */
#endif // __PROPHET_PROTOCOL_ANALYZERS_SEPARATOR_H__