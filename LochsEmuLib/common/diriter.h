#pragma once

#ifndef __COMMON_DIRITER_H__
#define __COMMON_DIRITER_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

class LX_API DirectoryIterator
{
public:
    DirectoryIterator(const char * path, const char * match = NULL);
    virtual ~DirectoryIterator();

    WIN32_FIND_DATAA *	Get()	{ return &m_result; }
    void		        GetFullPath(char * out, uint outLen) const;
    std::string	        GetFullPath() const;
    std::string         GetFileName() const { return std::string(m_result.cFileName); }

    void	            Next();
    bool	            Done();

private:
    DirectoryIterator();	// undefined, disallow

    HANDLE			m_searchHandle;
    WIN32_FIND_DATAA	m_result;
    bool			m_done;

    char	m_path[MAX_PATH];
};

END_NAMESPACE_LOCHSEMU()

#endif // __COMMON_DIRITER_H__
