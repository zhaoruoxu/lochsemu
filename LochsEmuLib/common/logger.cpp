#include "stdafx.h"
#include "logger.h"

BEGIN_NAMESPACE_LOCHSEMU()

Logger::Logger() : pFile(NULL) {
}

Logger::~Logger() {
    char dt[64];
    LxGetDateTime(dt);
    Log("==========Log stopped: %s==========\n\n", dt);
    fclose(pFile);
}

void Logger::OpenLogFile(const char *filename, bool replaceExisting) {
    pFile = fopen(filename, replaceExisting ? "w" : "a");
    Assert(pFile);
    char dt[64];
    LxGetDateTime(dt);
    Log("==========Log started: %s==========\n", dt);
}

void Logger::Log(const char *fmt, ...) {
    Assert(NULL != pFile);

    va_list args;
    va_start(args, fmt);
    vfprintf(pFile, fmt, args);
    va_end(args);
    fflush(pFile);
}

END_NAMESPACE_LOCHSEMU()