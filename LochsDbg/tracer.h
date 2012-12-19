#pragma once

#ifndef __LOCHSDBG_TRACER_H__
#define __LOCHSDBG_TRACER_H__

#include "LochsDbg.h"

class Tracer {
public:
    Tracer();
    virtual ~Tracer();

    void    Initialize(LPCSTR lpFileName);
    void    Enable(bool enable) { m_enabled = enable; }
    bool    Enabled() const { return m_enabled; }
    void    Trace(const char *fmt, ...);
private:
    bool    m_enabled;
    FILE  * m_file;
};

#endif // __LOCHSDBG_TRACER_H__
