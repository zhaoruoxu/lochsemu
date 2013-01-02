#include "stdafx.h"
#include "diriter.h"

BEGIN_NAMESPACE_LOCHSEMU()

DirectoryIterator::DirectoryIterator(LPCTSTR path, LPCTSTR match)
:m_searchHandle(INVALID_HANDLE_VALUE), m_done(false)
{
    if(!match) match = _T("*");

    wcscpy_s(m_path, _countof(m_path), path);

    bool hasSlash = path[wcslen(path)-1] == _T('\\');

    TCHAR wildcardPath[MAX_PATH];
    if (hasSlash) {
        wsprintf(wildcardPath, _T("%s%s"), path, match);
    } else {
        wsprintf(wildcardPath, _T("%s\\%s"), path, match);
    }
    
    m_searchHandle = FindFirstFile(wildcardPath, &m_result);
    if(m_searchHandle == INVALID_HANDLE_VALUE)
        m_done = true;
}

DirectoryIterator::~DirectoryIterator()
{
    if(m_searchHandle != INVALID_HANDLE_VALUE)
        FindClose(m_searchHandle);
}

void DirectoryIterator::GetFullPath(char * out, uint outLen) const
{
    sprintf_s(out, outLen, "%s%s", m_path, m_result.cFileName);
}

std::wstring DirectoryIterator::GetFullPath() const
{
    return std::wstring(m_path) + std::wstring(m_result.cFileName);
}

void DirectoryIterator::Next()
{
    BOOL	result = FindNextFile(m_searchHandle, &m_result);
    if(!result)
        m_done = true;
}

bool DirectoryIterator::Done()
{
    return m_done;
}


DirectoryIteratorA::DirectoryIteratorA(LPCSTR path, LPCSTR match)
    :m_searchHandle(INVALID_HANDLE_VALUE), m_done(false)
{
    if(!match) match = "*";

    strcpy_s(m_path, _countof(m_path), path);

    bool hasSlash = path[strlen(path)-1] == '\\';

    CHAR wildcardPath[MAX_PATH];
    if (hasSlash) {
        sprintf(wildcardPath, "%s%s", path, match);
    } else {
        sprintf(wildcardPath, "%s\\%s", path, match);
    }

    m_searchHandle = FindFirstFileA(wildcardPath, &m_result);
    if(m_searchHandle == INVALID_HANDLE_VALUE)
        m_done = true;
}

DirectoryIteratorA::~DirectoryIteratorA()
{
    if(m_searchHandle != INVALID_HANDLE_VALUE)
        FindClose(m_searchHandle);
}

void DirectoryIteratorA::GetFullPath(char * out, uint outLen) const
{
    sprintf_s(out, outLen, "%s%s", m_path, m_result.cFileName);
}

std::string DirectoryIteratorA::GetFullPath() const
{
    return std::string(m_path) + std::string(m_result.cFileName);
}

void DirectoryIteratorA::Next()
{
    BOOL	result = FindNextFileA(m_searchHandle, &m_result);
    if(!result)
        m_done = true;
}

bool DirectoryIteratorA::Done()
{
    return m_done;
}


END_NAMESPACE_LOCHSEMU()