#include "stdafx.h"
#include "config.h"

BEGIN_NAMESPACE_LOCHSEMU()


const uint Config::BufSize = 40960;


Config::Config()
{
    m_initialized = false;
}

Config::~Config()
{
    m_initialized = false;
}


bool Config::Initialize( LPCSTR lpFileName )
{
    std::string fileName(lpFileName);
    std::string::size_type pos = fileName.rfind('\\');
    if (pos == std::string::npos) {
        m_fileName = LxGetRuntimeDirectory() + fileName;
    } else {
        m_fileName = fileName;
    }
    m_initialized = true;

    return LxFileExists(m_fileName.c_str());
}


std::string Config::GetString(LPCSTR lpSection, LPCSTR lpName, LPCSTR lpDefault /*= NULL*/)
{
    Assert(m_initialized);
    char buf[BufSize];
    DWORD dwBytes = GetPrivateProfileStringA(lpSection, lpName, lpDefault, 
        buf, BufSize, m_fileName.c_str());
    if (dwBytes == 0) {
        LxWarning("Config::GetString 0 bytes, Section=%s, Key=%s\n", lpSection, lpName);
    }
    if (lpDefault && !strcmp(buf, lpDefault)) {
        B( WritePrivateProfileStringA(lpSection, lpName, lpDefault, m_fileName.c_str()) );
    }
    return std::string(buf);
}

void Config::SetString(LPCSTR lpSection, LPCSTR lpName, LPCSTR lpValue)
{
    Assert(m_initialized);
    B( WritePrivateProfileStringA(lpSection, lpName, lpValue, m_fileName.c_str()) );
}

int Config::GetInt( LPCSTR lpSection, LPCSTR lpKey, int def /*= 0*/ )
{
    Assert(m_initialized);
    int r = GetPrivateProfileIntA(lpSection, lpKey, def, m_fileName.c_str());
    if (r == def) {
        char buf[BufSize];
        sprintf(buf, "%d", def);
        B( WritePrivateProfileStringA(lpSection, lpKey, buf, m_fileName.c_str()) );
    }
    return r;
}

void Config::SetInt( LPCSTR lpSection, LPCSTR lpKey, int def )
{
    Assert(m_initialized);
    char buf[BufSize];
    sprintf(buf, "%d", def);
    B( WritePrivateProfileStringA(lpSection, lpKey, buf, m_fileName.c_str()) );
}

uint Config::GetUint( LPCSTR lpSection, LPCSTR lpKey, uint def /*= 0*/ )
{
    Assert(m_initialized);
    char buf[BufSize];
    ultoa(def, buf, 16);
    std::string s = GetString(lpSection, lpKey, buf);
    return strtoul(s.c_str(), NULL, 16);
}

void Config::SetUint( LPCSTR lpSection, LPCSTR lpKey, uint def )
{
    Assert(m_initialized);
    char buf[BufSize];
    ultoa(def, buf, 16);
    SetString(lpSection, lpKey, buf);
}

double Config::GetDouble( LPCSTR lpSection, LPCSTR lpKey, double def /*= 0.0*/ )
{
    Assert(m_initialized);
    char buf[BufSize];
    sprintf(buf, "%f", def);
    std::string s = GetString(lpSection, lpKey, buf);
    return atof(s.c_str());
}

void Config::SetDouble( LPCSTR lpSection, LPCSTR lpKey, double val )
{
    Assert(m_initialized);
    char buf[BufSize];
    sprintf(buf, "%f", val);
    SetString(lpSection, lpKey, buf);
}

END_NAMESPACE_LOCHSEMU()