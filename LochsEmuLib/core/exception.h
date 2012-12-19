#pragma once

#ifndef __CORE_EXCEPTION_H__
#define __CORE_EXCEPTION_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

class ExceptionManager {
public:
    ExceptionManager();
    ~ExceptionManager();

    void        Initialize(Processor *cpu);
    void        Raise(DWORD code, DWORD flags = 0, DWORD numParams = 0, LPVOID params = NULL);

private:

    /*
     * Construct CONTEXT structure on stack
     * returns address of CONTEXT
     */
    u32         ConstructContext();

    /*
     * Restore processor context from memory
     */
    void        RestoreContext(u32 ctxAddress);

    /*
     * Construct ExceptionRecord structure on stack
     * returns the address of ExceptionRecord
     */
    u32         ConstructExceptionRecord(DWORD code, DWORD flags = 0, DWORD nParams = 0, 
                    LPVOID params = NULL, EXCEPTION_RECORD *chain = NULL );

    /*
     * Remove ExceptionRecord from stack
     */
    void        RemoveExceptionRecord(u32 addr);

    /*
     * Runs the exception handler as callback
     * Returns EXCEPTION_DISPOSITION enum
     */
    u32         RunExceptionHandler(u32 entry, u32 recordAddr, u32 ctxAddr, u32 dispCtxAddr);
private:
    Processor * m_cpu;
    bool        m_enabled;
};


END_NAMESPACE_LOCHSEMU()

#endif // __CORE_EXCEPTION_H__
