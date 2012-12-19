#include "stdafx.h"
#include "tracer.h"

Tracer::Tracer()
{
    m_enabled   = false;
    m_file      = NULL;
}

Tracer::~Tracer()
{
    if (m_file) {
        fclose(m_file);
    }
}

void Tracer::Initialize( LPCSTR lpFileName )
{
    m_enabled = g_config.GetInt("Tracer", "EnabledOnStart", false) != 0;
    m_file = fopen(lpFileName, "w");
    if (!m_file) {
        StdError("Error opening trace file: %s", lpFileName);
    }
}

void Tracer::Trace( const char *fmt, ... )
{
    va_list args;
    va_start(args, fmt);
    vfprintf(m_file, fmt, args);
    va_end(args);

    fflush(m_file);
}
