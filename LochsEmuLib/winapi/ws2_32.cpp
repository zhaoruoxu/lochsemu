#include "stdafx.h"
#include "winapi.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Ws2_32_accept(Processor *cpu)
{
    RET_VALUE = (u32) accept(
        (SOCKET)        PARAM(0),
        (sockaddr *)    PARAM_PTR(1),
        (int *)         PARAM_PTR(2)
        );
    RET_PARAMS(3);
}

uint Ws2_32_bind(Processor *cpu)
{
    RET_VALUE = (u32) bind(
        (SOCKET)        PARAM(0),
        (const sockaddr *)      PARAM_PTR(1),
        (int)           PARAM(2)
        );
    RET_PARAMS(3);
}

uint Ws2_32_closesocket(Processor *cpu)
{
    RET_VALUE = (u32) closesocket(
        (SOCKET)        PARAM(0)
        );
    RET_PARAMS(1);
}

uint Ws2_32_connect(Processor *cpu)
{
    RET_VALUE = (u32) connect(
        (SOCKET)        PARAM(0),
        (const sockaddr *)  PARAM_PTR(1),
        (int)           PARAM(2)
        );
    RET_PARAMS(3);
}

uint Ws2_32_htonl(Processor *cpu)
{
    Assert(sizeof(u_long) == 4);
    RET_VALUE = (u32) htonl(
        (u_long)        PARAM(0)
        );
    RET_PARAMS(1);
}

uint Ws2_32_htons(Processor *cpu)
{
    Assert(sizeof(u_short) == 2);
    RET_VALUE = (u32) htons(
        (u_short)       PARAM(0)
        );
    RET_PARAMS(1);
}

uint Ws2_32_inet_addr(Processor *cpu)
{
    RET_VALUE  = (u32) inet_addr(
        (const char *)  PARAM_PTR(0)
        );
    RET_PARAMS(1);
}

uint Ws2_32_listen(Processor *cpu)
{
    RET_VALUE = (u32) listen(
        (SOCKET)        PARAM(0),
        (int)           PARAM(1)
        );
    RET_PARAMS(2);
}

uint Ws2_32_recv(Processor *cpu)
{
    RET_VALUE = (u32) recv(
        (SOCKET)        PARAM(0),
        (char *)        PARAM_PTR(1),
        (int)           PARAM(2),
        (int)           PARAM(3)
        );
    RET_PARAMS(4);
}

uint Ws2_32_send(Processor *cpu)
{
    RET_VALUE = (u32) send(
        (SOCKET)        PARAM(0),
        (const char *)  PARAM_PTR(1),
        (int)           PARAM(2),
        (int)           PARAM(3)
        );
    RET_PARAMS(4);
}

uint Ws2_32_socket(Processor *cpu)
{
    RET_VALUE = (u32) socket(
        (int)           PARAM(0),
        (int)           PARAM(1),
        (int)           PARAM(2)
        );
    RET_PARAMS(3);
}

uint Ws2_32_WSAStartup(Processor *cpu)
{
    RET_VALUE = (u32) WSAStartup(
        (WORD)          PARAM(0),
        (LPWSADATA)     PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

uint Ws2_32_WSACleanup(Processor *cpu)
{
    RET_VALUE = (u32) WSACleanup();
    RET_PARAMS(0);
}

END_NAMESPACE_LOCHSEMU()
