#pragma once
 
#ifndef __PROPHET_PLUGIN_REMOTEDIFF_PIPESERVER_H__
#define __PROPHET_PLUGIN_REMOTEDIFF_PIPESERVER_H__
 
#include "Prophet.h"

enum SyncEvent {
    SE_Invalid = 0,
    SE_SingleStep,
    SE_Context,
    SE_ProcessCreate,
    SE_ProcessExit,
    SE_ThreadCreate,
    SE_ThreadExit,
};



struct SingleStepData {
    u32 ThreadId;
    u32 Eip;
    bool MultiInsts;
};

struct ContextData {
    u32 Tid;
    u32 Eip;
    u32 Esp;
};

struct ThreadCreateData {
    u32 ParentTid;
    u32 Tid;
    u32 StartAddress;
    u32 Esp;
};

struct ProcessCreateData {
    u32 ImageBase;
    u32 StartAddress;
    u32 ThreadId;
    u32 Esp;
};

struct SyncData {
    SyncEvent Event;
    union {
        SingleStepData      SingleStep;
        ContextData         Context;
        ThreadCreateData    ThreadCreate;
        ProcessCreateData   ProcessCreate;
    };

    SyncData(SyncEvent e) : Event(e) {}
};

class PipeServer {
public:
    PipeServer();
    ~PipeServer();

    bool        Connect(const char *pipename);

    std::string ReadString();
    void        WriteString(const char *s);
    template <typename T>
    T           ReadData();
    template <typename T>
    void        WriteData(const T &t);

private:
    static const int BufferSize = 256;

private:
    HANDLE      m_hPipe;
    char        m_buffer[BufferSize];

};

template <typename T>
void PipeServer::WriteData( const T &t )
{
    ZeroMemory(m_buffer, BufferSize);
    memcpy(m_buffer, &t, sizeof(T));
    
    DWORD dwWritten;
    WriteFile(m_hPipe, m_buffer, sizeof(T), &dwWritten, NULL);
}

template <typename T>
T PipeServer::ReadData()
{
    DWORD dwRead;
    uint size = sizeof(T);
    ReadFile(m_hPipe, m_buffer, sizeof(T), &dwRead, NULL);
    T t(SE_Invalid);
    memcpy(&t, m_buffer, sizeof(T));
    return t;
}


#endif // __PROPHET_PLUGIN_REMOTEDIFF_PIPESERVER_H__