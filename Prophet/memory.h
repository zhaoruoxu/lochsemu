#pragma once
 
#ifndef __PROPHET_MEMORY_H__
#define __PROPHET_MEMORY_H__
 
#include "prophet.h"

template <typename T>
class AllocOnlyPool {
public:
    AllocOnlyPool(int initialSize)
    {
        m_currSize      = 0;
        m_currMaxSize   = initialSize;
        m_memory        = new T[m_currMaxSize];
    }

    ~AllocOnlyPool()
    {
        SAFE_DELETE_ARRAY(m_memory);
        for (auto &t : m_storage) {
            SAFE_DELETE_ARRAY(t);
        }
        m_storage.clear();
    }

    T *     Alloc()
    {
        Assert(m_currSize < m_currMaxSize);
        T *res = &m_memory[m_currSize++];
        if (m_currSize == m_currMaxSize)
            Expand();
        return res;
    }

private:
    void    Expand()
    {
        m_storage.push_back(m_memory);
        m_currMaxSize *= 2;
        m_memory = new T[m_currMaxSize];
        m_currSize = 0;
    }
private:
    std::vector<T *>    m_storage;
    T *     m_memory;
    int     m_currSize;
    int     m_currMaxSize;
};


template <typename T>
class FactoryImpl {
public:
    FactoryImpl() { m_storage.reserve(16); }
    ~FactoryImpl() 
    {
        for (auto &t : m_storage) {
            SAFE_DELETE(t);
        }
        m_storage.clear();
    }

    T* Keep(T *p)
    {
        m_storage.push_back(p);
        return p;
    }

    int Size() const { return m_storage.size(); }

private:
    std::vector<T *>    m_storage;

    FactoryImpl(const FactoryImpl &);
    FactoryImpl& operator=(const FactoryImpl &);
};

class MemoryMappedPool {
public:
    MemoryMappedPool(const char *fileName);
    ~MemoryMappedPool();

    pbyte       Alloc(uint nBytes);
    template <typename T>
    T*          Alloc();
    void        Clear();

private:
    HANDLE      m_hFile;
    HANDLE      m_hMapFile;

    pbyte       m_pData;
    uint        m_currPtr;
};

template <typename T>
T* MemoryMappedPool::Alloc()
{
    return (T*) Alloc(sizeof(T));
}

template <typename T>
class FilePool {
public:
    FilePool()
    {
        m_hFile = INVALID_HANDLE_VALUE;
        m_ptr = 0;
    }

    ~FilePool()
    {
        if (m_hFile != INVALID_HANDLE_VALUE)
            CloseHandle(m_hFile);

        //DeleteFileA(m_fileName);
    }

    void    Init(const char *fileName)
    {
        strcpy(m_fileName, fileName);
        m_hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 
            0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (INVALID_HANDLE_VALUE == m_hFile) {
            LxFatal("Cannot create file pool: %s\n", fileName);
        }
    }

    u32     Alloc()
    {
        T dummy;
        return Alloc(&dummy);
    }

    u32     Alloc(const T *data)
    {
        Write(m_ptr, data);
        return m_ptr++; // index starts at 1
    }

    void    Read(u32 index, T *data) const
    {
        Seek(index);    // index starts at 1
        DWORD dwByteRead;
        if (ReadFile(m_hFile, data, sizeof(T), &dwByteRead, NULL) == FALSE) {
            LxFatal("Error reading file: %d\n", GetLastError());
        }
        if (dwByteRead != sizeof(T)) {
            LxFatal("Read size mismatch\n");
        }
    }

    void    Write(u32 index, const T *data) const
    {
        Seek(index); // index starts at 1
        DWORD dwByteWritten;
        if (WriteFile(m_hFile, data, sizeof(T), &dwByteWritten, NULL) == FALSE) {
            LxFatal("Error writing file: %d\n", GetLastError());
        }
        if (dwByteWritten != sizeof(T)) {
            LxFatal("Write size mismatch\n");
        }
    }

    void    Clear()
    {
        m_ptr = 0;
    }

private:
    void    Seek(u32 index) const
    {
        u64 p = sizeof(T) * (u64) index;
        SetFilePointer(m_hFile, (LONG) p, ((PLONG) &p) + 1, FILE_BEGIN);
        if (GetLastError() != 0) {
            LxFatal("Error SetFilePointerEx(%x): error code = %d\n", index, GetLastError());
        }
    }
private:
    HANDLE      m_hFile;
    u32         m_ptr;
    char        m_fileName[MAX_PATH];
};

#endif // __PROPHET_MEMORY_H__