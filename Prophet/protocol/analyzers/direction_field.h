#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__
#define __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__
 
#include "protocol/analyzer.h"
#include "instruction.h"

class DirectionField : public ProtocolAnalyzer {
public:
    DirectionField(Protocol *protocol);

    void    Initialize() override;
    void    OnPreExecute(PreExecuteEvent &event) override;
    void    OnMessageBegin(MessageBeginEvent &event) override;
    void    OnMessageEnd(MessageEndEvent &event) override;

    void    Serialize(Json::Value &root) const override;
    void    Deserialize(Json::Value &root) override;

private:
    bool    CheckMemoryLength(PreExecuteEvent &event, const ARGTYPE &arg);
    void    CheckFlag(PreExecuteEvent &event);

private:
    Disassembler *      m_disasm;
    //TaintEngine *       m_taint;
    MessageManager *    m_msgmgr;

    std::unordered_set<u32>     m_inloop;

    bool                m_useFlag;
};

 
#endif // __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__