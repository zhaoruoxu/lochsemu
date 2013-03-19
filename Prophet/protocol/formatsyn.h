#pragma once
 
#ifndef __PROPHET_PROTOCOL_FORMATSYN_H__
#define __PROPHET_PROTOCOL_FORMATSYN_H__

#include "prophet.h"
#include "utilities.h"
#include "event.h"

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

private:
    MessageManager *    m_msgmgr;
    TaintEngine *   m_taint;
};

#endif // __PROPHET_PROTOCOL_FORMATSYN_H__