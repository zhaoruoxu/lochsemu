#pragma once

#ifndef __COMMON_DIRITER_H__
#define __COMMON_DIRITER_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

class LX_API DirectoryIterator
{
public:
    DirectoryIterator(LPCTSTR path, LPCTSTR match = NULL);
    virtual ~DirectoryIterator();

    WIN32_FIND_DATA *	Get()	{ return &m_result; }
    void		        GetFullPath(char * out, uint outLen) const;
    std::wstring	    GetFullPath() const;
    LPCTSTR             GetFileName() const { return m_result.cFileName; }

    void	            Next();
    bool	            Done();

private:
    DirectoryIterator();	// undefined, disallow

    HANDLE			m_searchHandle;
    WIN32_FIND_DATA	m_result;
    bool			m_done;

    TCHAR	m_path[MAX_PATH];
};

class LX_API DirectoryIteratorA
{
public:
    DirectoryIteratorA(LPCSTR path, LPCSTR match = NULL);
    virtual ~DirectoryIteratorA();

    WIN32_FIND_DATAA*	Get()	{ return &m_result; }
    void		        GetFullPath(char * out, uint outLen) const;
    std::string	        GetFullPath() const;
    LPCSTR              GetFileName() const { return m_result.cFileName; }

    void	            Next();
    bool	            Done();

private:
    DirectoryIteratorA();	// undefined, disallow

    HANDLE			m_searchHandle;
    WIN32_FIND_DATAA	m_result;
    bool			m_done;

    CHAR	m_path[MAX_PATH];
};

END_NAMESPACE_LOCHSEMU()

#endif // __COMMON_DIRITER_H__
