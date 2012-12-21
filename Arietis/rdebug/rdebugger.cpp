#include "stdafx.h"
#include "rdebugger.h"

#define MSG_TERMINATOR  "\n\n"

RDebugger::RDebugger()
{
    m_state = DISCONNECTED;
}

RDebugger::~RDebugger()
{
    SendString("bye");
    closesocket(m_socket);
    WSACleanup();
}

bool RDebugger::Connect(const std::string &addr, uint port)
{
    Assert(m_state == DISCONNECTED);

    m_port = port;
    m_serverAddr = addr;

    WSADATA wsaData;
    SOCKADDR_IN serverAddr;

    WSAStartup(MAKEWORD(1, 1), &wsaData);

    m_socket                = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family   = AF_INET;
    serverAddr.sin_port     = htons(m_port);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(m_serverAddr.c_str());

    LxInfo("Arietis: connecting to remote debugger\n");
    if (connect(m_socket, (SOCKADDR *) &serverAddr, sizeof(serverAddr))) {
        LxError("Connection failed, error = %d\n", WSAGetLastError());
        return false;
    }

    char buf[256];
    sprintf_s(buf, 256, "arietis %x", ArietisVersion);

    if (!SendString(buf)) {
        LxFatal("Unable to establish connection\n");
        return false;
    }

    std::string respond = ReceiveString();
    
    if (respond == "hello") {
        LxInfo("Arietis: connection established\n");
    } else {
        LxFatal("Unable to establish connection, server says: %s\n", respond.c_str());
    }

    return m_state == CONNECTED;
}

bool RDebugger::SendString( const char *str )
{
    //if (m_state != CONNECTED) return false;

    std::string msg(str);
    msg += MSG_TERMINATOR;

    uint ptr = 0;
    char buf[BufferSize];

    while (ptr < msg.length()) {
        ZeroMemory(buf, BufferSize);
        memcpy(buf, msg.c_str() + ptr, min(BufferSize, msg.length() - ptr));
        if (!SendBuffer(buf, BufferSize)) return false;
        ptr += BufferSize;
    }

    return true;
}

std::string RDebugger::ReceiveString()
{
    std::string msg;
    char buf[BufferSize];

    while (true) {
        ZeroMemory(buf, BufferSize);
        recv(m_socket, buf, BufferSize, 0);
        const char *term = strstr(buf, MSG_TERMINATOR);
        if (term) {
            buf[term - buf] = '\0';
            msg += buf;
            break;
        }
        msg += buf;
    }
    return msg;
}

bool RDebugger::SendBuffer(const char *buf, int len)
{
    //Assert(m_state == CONNECTED);

    int nret = send(m_socket, buf, len, 0);

    //char buf[256];
    //ZeroMemory(buf, 256);
    //nret = recv(m_socket, buf, 256, 0);
    //if (strcmp(buf, "okay")) {
    //    LxError("Connection error, received: %s\n", buf);
    //    return false;
    //}

    return nret <= BufferSize;
}
