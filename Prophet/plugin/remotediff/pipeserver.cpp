#include "stdafx.h"
#include "pipeserver.h"


PipeServer::PipeServer()
{
    m_hPipe = INVALID_HANDLE_VALUE;
}

PipeServer::~PipeServer()
{
    if (m_hPipe != INVALID_HANDLE_VALUE)
        CloseHandle(m_hPipe);
}

bool PipeServer::Connect( const char *pipename )
{
    m_hPipe = CreateNamedPipeA(pipename, PIPE_ACCESS_DUPLEX, PIPE_WAIT, 1, 
        BufferSize, BufferSize, 1000, NULL);

    if (INVALID_HANDLE_VALUE == m_hPipe) {
        LxError("Cannot create pipe %s\n", pipename);
        return false;
    }

    if (!ConnectNamedPipe(m_hPipe, NULL)) {
        LxError("Cannot connect to pipe %s\n", pipename);
        return false;
    }

    const char * const HelloString = "hello";

    WriteString(HelloString);

    std::string s = ReadString();

    if (s == HelloString) {
        LxInfo("RemoteDiff: ClientHello\n");
        LxInfo("RemoteDiff: Pipe connection established\n");
        return true;
    } else {
        LxError("RemoteDiff: Pipe connection failed\n");
        return false;
    }
}

std::string PipeServer::ReadString()
{
    DWORD dwRead;
    ReadFile(m_hPipe, m_buffer, BufferSize, &dwRead, NULL);
    if (dwRead >= BufferSize) {
        LxFatal("Buffer overflow\n");
    }
    m_buffer[dwRead] = '\0';
    return std::string(m_buffer);
}

void PipeServer::WriteString( const char *s )
{
    if (strlen(s) >= BufferSize) {
        LxFatal("Buffer overflow\n");
    }
    ZeroMemory(m_buffer, BufferSize);
    strcpy(m_buffer, s);
    DWORD dwWritten;
    WriteFile(m_hPipe, m_buffer, BufferSize, &dwWritten, NULL);
}

