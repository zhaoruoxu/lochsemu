#include "stdafx.h"
#include "msgaccess.h"


MessageAccessLog::MessageAccessLog( CallStack *cs, const Message *msg )
{
    m_callstack = cs;
    m_currmsg = msg;
}

MessageAccessLog::~MessageAccessLog()
{
    Reset();
}

void MessageAccessLog::Reset()
{
    // m_callstack->Reset();
    for (auto &t : m_accesses) {
        SAFE_DELETE(t);
    }
    m_accesses.clear();
}

void MessageAccessLog::OnExecuteTrace( ExecuteTraceEvent &event )
{
    // m_callstack->OnExecuteTrace(event);
    const TContext *ctx = event.Context;

    if (ctx->Mr.Len == 0) {
        Assert(ctx->Mr.Addr == 0);
        return;
    }
    Assert(ctx->Mr.Len <= 4);
    pbyte data = (pbyte) &ctx->Mr.Val;
    for (u32 i = 0; i < ctx->Mr.Len; i++) {
        OnMemRead(ctx, ctx->Mr.Addr + i, data[i]);
    }
}

void MessageAccessLog::OnComplete()
{

}

void MessageAccessLog::OnMemRead( const TContext *t, u32 addr, byte data )
{
    if (addr < m_currmsg->Base() || addr >= m_currmsg->Base() + m_currmsg->Size())
        return;
    u32 offset = addr - m_currmsg->Base();
    if (data != (*m_currmsg)[offset].Data) {
        //LxWarning("Message data differs from original\n");
        return;
    }
    MessageAccess *acc = new MessageAccess;
    acc->CallStack = m_callstack->Get();
    acc->Context = t;
    acc->Offset = offset;
    m_accesses.push_back(acc);

    //printf("MSGACC %d %c %08x %s\n", offset, data, t->Eip, t->Inst->Main.CompleteInstr);
}

void MessageAccessLog::Dump( File &f ) const
{
    fprintf(f.Ptr(), "msg: '%s'\n", m_currmsg->ToString().c_str());
    for (auto &m : m_accesses) {
        char c = (*m_currmsg)[m->Offset].Data;
        fprintf(f.Ptr(), "%3d '%c' %08x %-50s  stack_hash=%08x", 
            m->Offset, isprint(c) ? c : '.', m->Context->Eip, 
            m->Context->Inst->Main.CompleteInstr, GetProcStackHash(m->CallStack));
        fprintf(f.Ptr(), "  %08x", m->CallStack[0]->Entry());
        for (uint i = 1; i < m->CallStack.size(); i++)
            fprintf(f.Ptr(), "->%08x", m->CallStack[i]->Entry());
        fprintf(f.Ptr(), "\n");
    }
    
}
