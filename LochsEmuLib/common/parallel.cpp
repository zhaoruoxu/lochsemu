#include "stdafx.h"
#include "parallel.h"

BEGIN_NAMESPACE_LOCHSEMU()

Mutex::Mutex( bool initialOwner )
{
    m_hMutex = CreateMutex(NULL, initialOwner, NULL);
    if (INVALID_HANDLE_VALUE == m_hMutex) {
        LxFatal("Error creating mutex\n");
    }
}

Mutex::~Mutex()
{
    CloseHandle(m_hMutex);
}

void Mutex::Wait( int t ) const
{
    WaitForSingleObject(m_hMutex, t);
}

void Mutex::Release() const
{
    ReleaseMutex(m_hMutex);
}

Semaphore::Semaphore()
{
    m_semaphore = CreateSemaphore(NULL, 0, 65535, NULL);
    if (INVALID_HANDLE_VALUE == m_semaphore) {
        LxFatal("Error creating semaphore\n");
    }
}

Semaphore::~Semaphore()
{
    CloseHandle(m_semaphore);
}

void Semaphore::Post( int count /*= 1*/ )
{
    if (!ReleaseSemaphore(m_semaphore, count, NULL)) {
        LxFatal("Error releasing semaphore\n");
    }
}

void Semaphore::Wait()
{
    if (WaitForSingleObject(m_semaphore, INFINITE) == WAIT_FAILED) {
        LxFatal("Error waiting for semaphore\n");
    }
}

bool Semaphore::TryWait()
{
    return WaitForSingleObject(m_semaphore, 0) == WAIT_OBJECT_0;
}

// MutexCS * MutexCS::Create()
// {
//     return new MutexCS;
// }
// 
// void MutexCS::Destroy( MutexCS *m )
// {
//     delete m;
// }

MutexCS::MutexCS()
{
    InitializeCriticalSection(&m_criticalSection);
}

MutexCS::~MutexCS()
{
    DeleteCriticalSection(&m_criticalSection);
}

MutexCSLock::MutexCSLock( MutexCS &m ) : m_mutex(m)
{
    EnterCriticalSection(&m_mutex.m_criticalSection);
}

MutexCSLock::~MutexCSLock()
{
    LeaveCriticalSection(&m_mutex.m_criticalSection);
}

MutexLock::MutexLock( Mutex &m )
    : m_mutex(m)
{
    m_mutex.Wait();
}

// MutexLock::MutexLock( MutexSyncObject &obj )
//     : m_mutex(obj.m_mutex)
// {
//     m_mutex.Wait();
// }

MutexLock::~MutexLock()
{
    m_mutex.Release();
}


MutexSyncObject::MutexSyncObject()
    : m_mutex(false)
{

}


SyncObjectLock::SyncObjectLock( const ISyncObject &obj )
    : m_obj(obj)
{
    m_obj.Lock();
}

SyncObjectLock::SyncObjectLock( ISyncObject &obj )
    : m_obj(obj)
{
    m_obj.Lock();
}

SyncObjectLock::~SyncObjectLock()
{
    m_obj.Unlock();
}

END_NAMESPACE_LOCHSEMU()