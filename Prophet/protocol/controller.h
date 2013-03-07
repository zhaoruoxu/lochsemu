#pragma once
 
#ifndef __PROPHET_PROTOCOL_CONTROLLER_H__
#define __PROPHET_PROTOCOL_CONTROLLER_H__

#include "Prophet.h"
#include "utilities.h"

class Controller : public ISerializable {
public:
    Controller(Protocol *protocol);
    ~Controller();

    void    Initialize();
    void    Serialize(Json::Value &root) const override {}
    void    Deserialize(Json::Value &root) override {}

private:
    Protocol *      m_protocol;
};

#endif // __PROPHET_PROTOCOL_CONTROLLER_H__