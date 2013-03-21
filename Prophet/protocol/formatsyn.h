#pragma once
 
#ifndef __PROPHET_PROTOCOL_FORMATSYN_H__
#define __PROPHET_PROTOCOL_FORMATSYN_H__

#include "prophet.h"
#include "utilities.h"
#include "event.h"
#include "apiprocessor.h"
#include "message.h"

struct MsgByteInfo {
    int     FormatCount[FieldFormat::Total];
    int     Target;

    MsgByteInfo();
};

// Format Synthesizer
class FormatSyn : public ISerializable {
public:
    FormatSyn(MessageManager *msgmgr);
    ~FormatSyn();

    void    Initialize();

    void    OnMessageBegin(MessageBeginEvent &event);
    void    OnMessageEnd(MessageEndEvent &event);

    void    Serialize(Json::Value &root) const override;
    void    Deserialize(Json::Value &root) override;

    void    SubmitLengthField(int first, int last, int target);
    void    SubmitToken(byte t, int first, int last);
    void    SubmitFixedLenField(int first, int last);
private:
    void    Synthesize();

private:
    Message *       m_message;
    MsgByteInfo *   m_msgInfo;
    MessageManager *    m_msgmgr;
    TaintEngine *   m_taint;

    int     m_weightLenField;
    int     m_weightLenFieldVar;
    int     m_weightLenFieldNoTarget;
    int     m_weightSeparator;
    int     m_weightSeparatedVar;
    int     m_weightFixedLen;
};

#endif // __PROPHET_PROTOCOL_FORMATSYN_H__