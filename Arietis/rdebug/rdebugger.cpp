#include "stdafx.h"
#include "rdebugger.h"

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

    ZeroMemory(m_buffer, sizeof(char) * BufferSize);
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
    ZeroMemory(m_buffer, BufferSize);
    sprintf_s(m_buffer, BufferSize, "arietis %x\n", ArietisVersion);
    send(m_socket, m_buffer, BufferSize, 0);

    ZeroMemory(m_buffer, BufferSize);

    int nret = recv(m_socket, m_buffer, BufferSize, 0);

    if (!strcmp(m_buffer, "hello")) {
        LxInfo("Arietis: connected to remote debugger\n");
        m_state = CONNECTED;
    } else {
        LxError("Connection failed, server reply: %s\n", m_buffer);
    }

    return m_state == CONNECTED;
}

bool RDebugger::Send( const char *buf, int len )
{
    return false;
}

bool RDebugger::SendString( const char *str )
{
    if (m_state != CONNECTED) return false;

    Assert(strlen(str) < BufferSize-1);
    ZeroMemory(m_buffer, BufferSize);

    strcpy(m_buffer, str);
    strcat(m_buffer, "\n");

    send(m_socket, m_buffer, BufferSize, 0);
    return true;
}
