#pragma once

#ifndef __COMMON_SINGLETON_H__
#define __COMMON_SINGLETON_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

template <typename T>
class Singleton {
public:
    static T* Instance() {
        if (NULL == instance.get()) instance.reset(new T);
        Assert(NULL != instance.get());
        return instance.get();
    }

protected:
    Singleton();
    ~Singleton();
private:
    Singleton(const Singleton &);
    Singleton &operator=(const Singleton &);
    static std::auto_ptr<T> instance;
};

template <class T> std::auto_ptr<T> Singleton<T>::instance;

END_NAMESPACE_LOCHSEMU()

#endif // __COMMON_SINGLETON_H__
