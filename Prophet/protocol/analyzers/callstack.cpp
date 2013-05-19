#include "stdafx.h"
#include "callstack.h"
#include "processor.h"

CallStack::CallStack(ProcScope *procs)
{
    m_procs = procs;
    Reset();
}

CallStack::~CallStack()
{
    Reset();
}

void CallStack::OnExecuteTrace( ExecuteTraceEvent &event )
{
    const TContext *ctx = event.Context;

    if (m_stack.empty() ||
        (m_prev && Instruction::IsCall(m_prev->Inst) && 
        !m_prev->HasExecFlag(LX_EXEC_WINAPI_CALL) && 
        !m_prev->HasExecFlag(LX_EXEC_WINAPI_JMP)) ) 
    {
        Procedure *p;
        if ((p = m_procs->Get(event.Context->Eip)) != NULL) {
            m_stack.push_back(p);
            //printf("%d : push %08x, stack size = %d\n", event.Seq, p->Entry(), m_stack.size());
        }
    }

    if (Instruction::IsRet(event.Context->Inst)) {
        m_stack.pop_back();
        //printf("%d : pop, stack size = %d\n", event.Seq, m_stack.size());
    }
    m_prev = ctx;
}

void CallStack::Reset()
{
    m_stack.clear();
    m_prev = NULL;
}

u32 GetProcStackHash( const ProcStack &stack )
{
    u32 hash = 0;
    for (auto &t : stack) {
        hash = hash * 13131 + (*t).Entry();
    }
    return hash;
}