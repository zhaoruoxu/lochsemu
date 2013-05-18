#include "stdafx.h"
#include "winapi.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

uint Ws2_32___WSAFDIsSet(Processor *cpu)
{
    RET_VALUE = (u32) __WSAFDIsSet(
        (SOCKET)        PARAM(0),
        (fd_set *)      PARAM_PTR(1)
        );
    RET_PARAMS(2);
}

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

uint Ws2_32_freeaddrinfo(Processor *cpu)
{
//     freeaddrinfo(
//         (PADDRINFOA)    PARAM_PTR(0)
//         );
    LxError("TODO: freeaddrinfo()\n");
    RET_PARAMS(1);
}

uint Ws2_32_getaddrinfo(Processor *cpu)
{
    PCSTR p0 = (PCSTR) PARAM_PTR(0);
    PCSTR p1 = (PCSTR) PARAM_PTR(1);
    const ADDRINFOA * p2 = (const ADDRINFOA *) PARAM_PTR(2);
    PADDRINFOA * p3 = (PADDRINFOA *) PARAM_PTR(3);

    // p3 返回指向ADDRINFO链表的指针，保存于0x900000开始的内存中
    // | ADDRINFO | ai_canonname | ai_addr | ADDRINFO | ...

    RET_VALUE = (u32) getaddrinfo(p0, p1, p2, p3);

    static u32 Base = 0x900000;

    u32 s = 0;
    for (PADDRINFOA p = *p3; p != NULL; p = p->ai_next) {
        s += sizeof(addrinfo) + sizeof(sockaddr);// + strlen(p->ai_canonname) + 1 + sizeof(sockaddr);
        if (p->ai_canonname) s += strlen(p->ai_canonname) + 1;
    }

    s = RoundUp(s);

    SyncObjectLock lock(*cpu->Mem);

    Base = cpu->Mem->FindFreePages(Base, s);
    V( cpu->Mem->Alloc(SectionDesc("ADDRINFO_linked_list", cpu->GetCurrentModule()),
        Base, s, PAGE_READWRITE) );
    LxDebug("Allocated memory for getaddrinfo() at %08x, size %08x\n", Base, s);

    //PADDRINFOA emuMem = reinterpret_cast<PADDRINFOA>(cpu->Mem->GetRawData(Base));
    pbyte mem = cpu->Mem->GetRawData(Base);
    pbyte memstart = mem;
    for (PADDRINFOA p = *p3; p != NULL; p = p->ai_next) {
        // save current pointer to ADDRINFOA
        PADDRINFOA currPtr = reinterpret_cast<PADDRINFOA>(mem);

        // copy content of ADDRINFOA
        memcpy(currPtr, p, sizeof(ADDRINFOA));
        mem += sizeof(ADDRINFOA);

        // copy ai_canonname
        if (p->ai_canonname) {
            int lenCanonName = strlen(p->ai_canonname);
            memcpy(mem, p->ai_canonname, lenCanonName);
            mem[lenCanonName] = '\0';
            currPtr->ai_canonname = reinterpret_cast<char *>(Base + (mem - memstart));
            mem += lenCanonName+1;
        }

        // copy sockaddr
        memcpy(mem, p->ai_addr, sizeof(sockaddr));
        currPtr->ai_addr = reinterpret_cast<sockaddr *>(Base + (mem - memstart));
        mem += sizeof(sockaddr);
        
        // update next
        if (currPtr->ai_next != 0)
            currPtr->ai_next = reinterpret_cast<PADDRINFOA>(Base + (mem - memstart));
    }
    *p3 = reinterpret_cast<PADDRINFOA>(Base);

    Base += s;

    RET_PARAMS(4);
}

// uint Ws2_32_gethostbyname(Processor *cpu)
// {
//     hostent * h = gethostbyname(
//         (const char *)  PARAM_PTR(0)
//         );
//     RET_PARAMS(1);
// }

u32 MapHostent(hostent *t, Processor *cpu)
{
    uint len = sizeof(hostent);
    len += strlen(t->h_name) + 1;
    char **aliases = t->h_aliases;
    int aliasesCount = 0;
    while (*aliases++ != NULL) {
        len += strlen(*aliases) + 1;
        aliasesCount++;
    }
    len += sizeof(char *) * (aliasesCount+1);
    char **addrlist = t->h_addr_list;
    int addrlistCount = 0;
    while (*addrlist++ != NULL) {
        len += t->h_length;
        addrlistCount++;
    }
    len += sizeof(char *) * (addrlistCount+1);
    len = RoundUp(len);

    static u32 Base = 0x900000;
    SyncObjectLock lock(*cpu->Mem);
    u32 actualBase = cpu->Mem->FindFreePages(Base, len);
    V( cpu->Mem->Alloc(SectionDesc("hostent", cpu->GetCurrentModule()),
        actualBase, len, PAGE_READWRITE) );
    LxDebug("Allocated memory for hostent at %08x, size %08x\n", actualBase, len);
    Base += len;

    pbyte mem = cpu->Mem->GetRawData(actualBase);
    pbyte memstart = mem;
    ZeroMemory(mem, len);

    // copy struct itself
    hostent *dest = (hostent *) mem;
    memcpy(dest, t, sizeof(hostent));
    mem += sizeof(hostent);

    // copy h_name
    dest->h_name = (char *) (actualBase + (mem - memstart));
    strcpy((char *) mem, t->h_name);
    mem += strlen(t->h_name) + 1;

    // copy h_aliases
    char **taliases = (char **) mem;
    dest->h_aliases = (char **) (actualBase + (mem - memstart));
    mem += (aliasesCount + 1) * sizeof(char *);
    aliases = t->h_aliases;
    while (*aliases != NULL) {
        *taliases = (char *) (actualBase + (mem - memstart));
        strcpy((char *) mem, *aliases);
        mem += strlen(*aliases) + 1;
        aliases++;
        taliases++;
    }

    // copy h_addr_list
    char **taddrlist = (char **) mem;
    dest->h_addr_list = (char **) (actualBase + (mem - memstart));
    mem += (addrlistCount + 1) * sizeof(char *);
    addrlist = t->h_addr_list;
    while (*addrlist != NULL) {
        *taddrlist = (char *) (actualBase + (mem - memstart));
        memcpy(mem, *addrlist, t->h_length);
        mem += t->h_length;
        addrlist++;
        taddrlist++;
    }

    return actualBase;
}

uint Ws2_32_gethostbyaddr(Processor *cpu)
{
    hostent *r = gethostbyaddr(
        (const char *)      PARAM_PTR(0),
        (int)               PARAM(1),
        (int)               PARAM(2)
        );
    RET_VALUE = MapHostent(r, cpu);
    RET_PARAMS(3);
}

uint Ws2_32_gethostbyname(Processor *cpu)
{
    hostent *r = gethostbyname(
        (const char *)      PARAM_PTR(0)
        );
    RET_VALUE = MapHostent(r, cpu);
    RET_PARAMS(1);
}

uint Ws2_32_gethostname(Processor *cpu)
{
    RET_VALUE = (u32) gethostname(
        (char *)        PARAM_PTR(0),
        (int)           PARAM(1)
        );
    RET_PARAMS(2);
}

uint Ws2_32_getpeername(Processor *cpu)
{
    RET_VALUE = (u32) getpeername(
        (SOCKET)        PARAM(0),
        (sockaddr *)    PARAM_PTR(1),
        (int *)         PARAM_PTR(2)
        );
    RET_PARAMS(3);
}

uint Ws2_32_getsockname(Processor *cpu)
{
    RET_VALUE = (u32) getsockname(
        (SOCKET)        PARAM(0),
        (sockaddr *)    PARAM_PTR(1),
        (int *)         PARAM_PTR(2)
        );
    RET_PARAMS(3);
}

uint Ws2_32_getsockopt(Processor *cpu)
{
    RET_VALUE = (u32) getsockopt(
        (SOCKET)        PARAM(0),
        (int)           PARAM(1),
        (int)           PARAM(2),
        (char *)        PARAM_PTR(3),
        (int *)         PARAM_PTR(4)
        );
    RET_PARAMS(5);
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

uint Ws2_32_ioctlsocket(Processor *cpu)
{
    RET_VALUE = (u32) ioctlsocket(
        (SOCKET)        PARAM(0),
        (long)          PARAM(1),
        (u_long *)      PARAM_PTR(2)
        );
    RET_PARAMS(3);
}

uint Ws2_32_listen(Processor *cpu)
{
    RET_VALUE = (u32) listen(
        (SOCKET)        PARAM(0),
        (int)           PARAM(1)
        );
    RET_PARAMS(2);
}

uint Ws2_32_ntohs(Processor *cpu)
{
    RET_VALUE = (u32) ntohs(
        (u_short)       PARAM(0)
        );
    RET_PARAMS(1);
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

uint Ws2_32_recvfrom(Processor *cpu)
{
    RET_VALUE = (u32) recvfrom(
        (SOCKET)        PARAM(0),
        (char *)        PARAM_PTR(1),
        (int)           PARAM(2),
        (int)           PARAM(3),
        (sockaddr *)    PARAM_PTR(4),
        (int *)         PARAM_PTR(5)
        );
    RET_PARAMS(6);
}

uint Ws2_32_select(Processor *cpu)
{
    RET_VALUE = (u32) select(
        (int)           PARAM(0),
        (fd_set *)      PARAM_PTR(1),
        (fd_set *)      PARAM_PTR(2),
        (fd_set *)      PARAM_PTR(3),
        (const timeval *)   PARAM_PTR(4)
        );
    RET_PARAMS(5);
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

uint Ws2_32_sendto(Processor *cpu)
{
    RET_VALUE = (u32) sendto(
        (SOCKET)        PARAM(0),
        (const char *)  PARAM_PTR(1),
        (int)           PARAM(2),
        (int)           PARAM(3),
        (const sockaddr *)  PARAM_PTR(4),
        (int)           PARAM(5)
        );
    RET_PARAMS(6);
}

uint Ws2_32_setsockopt(Processor *cpu)
{
    RET_VALUE = (u32) setsockopt(
        (SOCKET)        PARAM(0),
        (int)           PARAM(1),
        (int)           PARAM(2),
        (const char *)  PARAM_PTR(3),
        (int)           PARAM(4)
        );
    RET_PARAMS(5);
}

uint Ws2_32_shutdown(Processor *cpu)
{
    RET_VALUE = (u32) shutdown(
        (SOCKET)        PARAM(0),
        (int)           PARAM(1)
        );
    RET_PARAMS(2);
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

uint Ws2_32_WSAGetLastError(Processor *cpu)
{
    RET_VALUE = (u32) WSAGetLastError();
    RET_PARAMS(0);
}

uint Ws2_32_WSASetLastError(Processor *cpu)
{
    WSASetLastError(
        (int)       PARAM(0)
        );
    RET_PARAMS(1);
}

END_NAMESPACE_LOCHSEMU()
