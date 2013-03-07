#pragma once
 
#ifndef __PROPHET_EVENT_H__
#define __PROPHET_EVENT_H__
 
#include "Prophet.h"

class Event {
public:
    Event(void *sender) : m_continue(true), m_sender(sender) {}
    virtual ~Event() {}

    void    Veto() { m_continue = false; }
    bool    IsVetoed() const { return !m_continue; }

    template <typename T>
    T *     GetSender() { return (T *) m_sender; }

    template <typename T>
    const T*GetSender() const { return (const T *) m_sender; }

private:
    bool    m_continue;
    void * const    m_sender;
};

enum EventHandlerFlag : uint {
    PreExecuteHandler       = 1,
    PostExecuteHandler      = 1 << 2,
    MemReadHandler          = 1 << 3,
    MemWriteHandler         = 1 << 4,
    ProcessPreRunHandler    = 1 << 5,
    ProcessPostRunHandler   = 1 << 6,
    ProcessPreLoadHandler   = 1 << 7,
    ProcessPostLoadHandler  = 1 << 8,
    WinapiPreCallHandler    = 1 << 9,
    WinapiPostCallHandler   = 1 << 10,
};

class PreExecuteEvent : public Event {
public:
    PreExecuteEvent(void *sender, Processor *cpu, const Instruction *inst)
        : Event(sender), Cpu(cpu), Inst(inst) {}
    
    Processor * const   Cpu;
    const Instruction * const Inst;
};

class PostExecuteEvent : public Event {
public:
    PostExecuteEvent(void *sender, Processor *cpu, const Instruction *inst)
        : Event(sender), Cpu(cpu), Inst(inst) {}

    Processor * const   Cpu;
    const Instruction * const Inst;
};

class MemReadEvent : public Event {
public:
    MemReadEvent(void *sender, const Processor *cpu, u32 addr, u32 nbytes, cpbyte data)
        : Event(sender), Cpu(cpu), Addr(addr), NBytes(nbytes), Data(data) {}

    const Processor * const Cpu;
    const u32               Addr;
    const u32               NBytes;
    cpbyte const            Data;
};

class MemWriteEvent : public Event {
public:
    MemWriteEvent(void *sender, const Processor *cpu, u32 addr, u32 nbytes, cpbyte data)
        : Event(sender), Cpu(cpu), Addr(addr), NBytes(nbytes), Data(data) {}

    const Processor * const Cpu;
    const u32               Addr;
    const u32               NBytes;
    cpbyte const            Data;
};

class ProcessPreRunEvent : public Event {
public:
    ProcessPreRunEvent(void *sender, const Process *proc, const Processor *cpu)
        : Event(sender), Proc(proc), Cpu(cpu) {}

    const Process * const   Proc;
    const Processor * const Cpu;
};

class ProcessPostRunEvent : public Event {
public:
    ProcessPostRunEvent(void *sender, const Process *proc)
        : Event(sender), Proc(proc) {}

    const Process * const   Proc;
};

class ProcessPreLoadEvent : public Event {
public:
    ProcessPreLoadEvent(void *sender, const PeLoader *loader)
        : Event(sender), Loader(loader) {}

    const PeLoader * const  Loader;
};

class ProcessPostLoadEvent : public Event {
public:
    ProcessPostLoadEvent(void *sender, const PeLoader *loader)
        : Event(sender), Loader(loader) {}
    
    const PeLoader * const  Loader;
};

class WinapiPreCallEvent : public Event {
public:
    WinapiPreCallEvent(void *sender, Processor *cpu, uint apiIndex)
        : Event(sender), Cpu(cpu), ApiIndex(apiIndex) {}

    Processor * const       Cpu;
    const uint              ApiIndex;
};

class WinapiPostCallEvent : public Event {
public:
    WinapiPostCallEvent(void *sender, Processor *cpu, uint apiIndex)
        : Event(sender), Cpu(cpu), ApiIndex(apiIndex) {}
    
    Processor * const       Cpu;
    const uint              ApiIndex;
};

#endif // __PROPHET_EVENT_H__