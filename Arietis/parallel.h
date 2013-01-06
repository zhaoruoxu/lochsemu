#pragma once

#ifndef __ARIETIS_PARALLEL_H__
#define __ARIETIS_PARALLEL_H__

#include "Arietis.h"

class Mutex {
public:
    Mutex(bool initialOwner);
    ~Mutex();

    void Wait(int t = INFINITE);
    void Release();
private:
    HANDLE m_hMutex;
};

class MutexCS {
public:
    static MutexCS *Create();
    static void Destroy(MutexCS *m);
private:
    MutexCS();
    ~MutexCS();
    friend class MutexCSLock;
    MutexCS(MutexCS &);
    MutexCS &operator=(const MutexCS &);

    CRITICAL_SECTION m_criticalSection;
};

class MutexCSLock {
public:
    MutexCSLock(MutexCS &m);
    ~MutexCSLock();
private:
    MutexCS &m_mutex;
    MutexCSLock(const MutexCSLock &);
    MutexCSLock &operator=(const MutexCSLock &);
};

class Semaphore {
public:
    Semaphore();
    ~Semaphore();
    void    Post(int count = 1);
    void    Wait();
    bool    TryWait();
private:
    HANDLE  m_semaphore;
};

#endif // __ARIETIS_PARALLEL_H__
