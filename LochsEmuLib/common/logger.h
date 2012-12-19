#pragma once

#ifndef __COMMON_LOGGER_H__
#define __COMMON_LOGGER_H__

#include "lochsemu.h"
#include "singleton.h"

BEGIN_NAMESPACE_LOCHSEMU()

class Logger {
public:
    Logger();
    virtual ~Logger();

    void    OpenLogFile(const char *filename, bool replaceExisting);
    void    Log(const char *fmt, ...);
private:
    FILE *  pFile;
};

typedef Singleton<Logger> Log;

END_NAMESPACE_LOCHSEMU()

#endif // __COMMON_LOGGER_H__
