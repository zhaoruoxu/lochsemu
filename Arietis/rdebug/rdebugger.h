#pragma once
 
#ifndef __RDEBUG_RDEBUGGER_H__
#define __RDEBUG_RDEBUGGER_H__
 
#include "Arietis.h"

class RDebugger {

private:
    enum State {
        DISCONNECTED,
        CONNECTED,
    };

public:
    RDebugger();
    ~RDebugger();

    bool Connect(const std::string &addr, uint port);
    bool SendString(const char *str);
    std::string ReceiveString();

private:
    static const int BufferSize = 4096;
    bool SendBuffer(const char *buf, int len);

private:
    State       m_state;
    SOCKET      m_socket;
    std::string m_serverAddr;
    uint        m_port;
};
 
#endif // __RDEBUG_RDEBUGGER_H__