#include "stdafx.h"
#include "diriter.h"

BEGIN_NAMESPACE_LOCHSEMU()

DirectoryIterator::DirectoryIterator(const char * path, const char * match)
:m_searchHandle(INVALID_HANDLE_VALUE), m_done(false)
{
    if(!match) match = "*";

    strcpy_s(m_path, sizeof(m_path), path);

    bool hasSlash = path[strlen(path)-1] == '\\';

    char	wildcardPath[MAX_PATH];
    if (hasSlash) {
        sprintf_s(wildcardPath, sizeof(wildcardPath), "%s%s", path, match);
    } else {
        sprintf_s(wildcardPath, sizeof(wildcardPath), "%s\\%s", path, match);
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

std::string DirectoryIterator::GetFullPath() const
{
    return std::string(m_path) + std::string(m_result.cFileName);
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


END_NAMESPACE_LOCHSEMU()