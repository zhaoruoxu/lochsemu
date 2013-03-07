#pragma once
 
#ifndef __PROPHET_PROTOCOL_PRETAINT_H__
#define __PROPHET_PROTOCOL_PRETAINT_H__
 
#include "Prophet.h"

class PreTainter {
public:
    PreTainter(Protocol *protocol);
    ~PreTainter();

    void        OnWinapiPostCall(WinapiPostCallEvent &event);

private:
    Protocol *      m_protocol;
};

#endif // __PROPHET_PROTOCOL_PRETAINT_H__