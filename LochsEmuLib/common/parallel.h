#pragma once
 
#ifndef __COMMON_PARALLEL_H__
#define __COMMON_PARALLEL_H__
 
#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()


class LX_API Mutex {
public:
    Mutex(bool initialOwner);
    ~Mutex();

    void Wait(int t = INFINITE) const;
    void Release() const;
private:
    HANDLE m_hMutex;
};

class LX_API MutexCS {
public:
//     static MutexCS *Create();
//     static void Destroy(MutexCS *m);
    MutexCS();
    ~MutexCS();
private:
    
    friend class MutexCSLock;
    MutexCS(MutexCS &);
    MutexCS &operator=(const MutexCS &);

    CRITICAL_SECTION m_criticalSection;
};

class LX_API MutexCSLock {
public:
    MutexCSLock(MutexCS &m);
    ~MutexCSLock();
private:
    MutexCS &m_mutex;
    MutexCSLock(const MutexCSLock &);
    MutexCSLock &operator=(const MutexCSLock &);
};

class LX_API Semaphore {
public:
    Semaphore();
    ~Semaphore();
    void    Post(int count = 1);
    void    Wait();
    bool    TryWait();
private:
    HANDLE  m_semaphore;
};


END_NAMESPACE_LOCHSEMU()
 
#endif // __COMMON_PARALLEL_H__