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
    if (m_stack.empty()) {
        OnProcBegin(event);
    }
}

void CallStack::Reset()
{
    m_stack.clear();
    //m_prev = NULL;
}

void CallStack::OnProcBegin( ExecuteTraceEvent &event )
{
    Procedure *p;
    if ((p = m_procs->Get(event.Context->Eip)) != NULL) {
        m_stack.push_back(p);
    }
}

void CallStack::OnProcEnd( ExecuteTraceEvent &event )
{
    m_stack.pop_back();

}

void CallStack::OnComplete()
{
    Reset();
}

u32 GetProcStackHash( const ProcStack &stack )
{
    u32 hash = 0;
    for (auto &t : stack) {
        hash = hash * 13131 + (*t).Entry();
    }
    return hash;
}
