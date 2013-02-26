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

class LX_API ISyncObject {
public:
    virtual void    Lock() const = 0;
    virtual void    Unlock() const = 0;
};

class LX_API MutexSyncObject : public ISyncObject {
    friend class MutexLock;
public:
    MutexSyncObject();

    void    Lock() const override 
    { 
        m_mutex.Wait(); 
    }
    
    void    Unlock() const override 
    { 
        m_mutex.Release(); 
    }
private:
    Mutex   m_mutex;
};

class LX_API SyncObjectLock {
public:
    SyncObjectLock(ISyncObject &obj);
    SyncObjectLock(const ISyncObject &obj);
    ~SyncObjectLock();
private:
    const ISyncObject &m_obj;
    SyncObjectLock(const SyncObjectLock &);
    SyncObjectLock &operator=(const SyncObjectLock &);
};

class LX_API MutexLock {
public:
    MutexLock(Mutex &m);
    //MutexLock(MutexSyncObject &obj);
    ~MutexLock();
private:
    Mutex &m_mutex;
    MutexLock(const MutexLock &);
    MutexLock &operator=(const MutexLock &);
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