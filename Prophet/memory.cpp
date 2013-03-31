#include "stdafx.h"
#include "memory.h"


MemoryMappedPool::MemoryMappedPool(const char *fileName)
{
    m_hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, 
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE == m_hFile) {
        LxFatal("Unable to create file: %s\n", fileName);
    }

    m_hMapFile = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, 0x40000000, NULL);
    if (NULL == m_hMapFile) {
        LxFatal("Unable to create mapping file\n");
    }

    m_pData = (pbyte) MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (NULL == m_pData) {
        LxFatal("Unable to map view of file\n");
    }
    m_currPtr = 0;
}

MemoryMappedPool::~MemoryMappedPool()
{
    if (!UnmapViewOfFile(m_pData)) {
        LxFatal("Unable to unmap view of file\n");
    }
    CloseHandle(m_hMapFile);
    CloseHandle(m_hFile);
}

pbyte MemoryMappedPool::Alloc( uint nBytes )
{
    pbyte pRet = m_pData + m_currPtr; 
    m_currPtr += nBytes;
    ZeroMemory(pRet, nBytes);
    return pRet;
}

void MemoryMappedPool::Clear()
{
    m_currPtr = 0;
}
