#pragma once

#ifndef __COMMON_CONFIG_H__
#define __COMMON_CONFIG_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

class LX_API Config {
public:
    Config();
    virtual ~Config();

    /*
     * if file exists, return true; otherwise, return false
     * if file not exists, create new on setting values
     */
    bool        Initialize  (LPCSTR lpFileName);
    std::string GetString   (LPCSTR lpSection, LPCSTR lpKey, LPCSTR lpDefault = NULL);
    void        SetString   (LPCSTR lpSection, LPCSTR lpKey, LPCSTR lpValue);
    int         GetInt      (LPCSTR lpSection, LPCSTR lpKey, int def = 0);
    void        SetInt      (LPCSTR lpSection, LPCSTR lpKey, int def);
    uint        GetUint     (LPCSTR lpSection, LPCSTR lpKey, uint def = 0);
    void        SetUint     (LPCSTR lpSection, LPCSTR lpKey, uint def);
    double      GetDouble   (LPCSTR lpSection, LPCSTR lpKey, double def = 0.0);
    void        SetDouble   (LPCSTR lpSection, LPCSTR lpKey, double val);

public:
    static const uint   BufSize;
private:
    bool        m_initialized;
    std::string m_fileName;
};

END_NAMESPACE_LOCHSEMU()

#endif // __COMMON_CONFIG_H__
