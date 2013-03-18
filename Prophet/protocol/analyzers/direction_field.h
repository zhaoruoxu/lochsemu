#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__
#define __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__
 
#include "protocol/analyzer.h"

class DirectionField : public ProtocolAnalyzer {
public:
    DirectionField(Protocol *protocol);

    void    Initialize() override;
    void    OnPreExecute(PreExecuteEvent &event) override;

private:
    TaintEngine *   m_taint;
};

 
#endif // __PROPHET_PROTOCOL_ANALYZERS_DIRECTION_FIELD_H__