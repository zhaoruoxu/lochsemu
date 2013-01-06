#pragma once

#ifndef __LOCHSDBG_DEBUGGER_H__
#define __LOCHSDBG_DEBUGGER_H__

#include "LochsDbg.h"
#include "instruction.h"
#include "tracer.h"

enum DebuggerState {
    STATE_SINGLESTEP,
    STATE_RUNNING,
    STATE_TERMINATED
};

struct MemAccessInfo {
    u32 offset;
    u128 simd;
    u64 value;
    u32 bytes;

    MemAccessInfo() {
        offset = bytes = 0;
        value = 0;
    }
    MemAccessInfo(u32 o, u64p v, u32 b) {
        offset = o; value = *v; bytes = b;
    }
    MemAccessInfo(u32 o, u128p v, u32 b) {
        offset = o; simd = *v; bytes = b;
    }
    void Dump() const {
        if (bytes == 16) {
            StdDumpDark("[%08x][%08x %08x %08x %08x]", offset,
                simd.dat[3], simd.dat[2], simd.dat[1], simd.dat[0]);
        } else if (bytes == 8) {
            StdDumpDark("[%08x][%I64x]", offset, value);
        } else if (bytes == 4) {
            StdDumpDark("[%08x][%08x]", offset, (u32) value);
        } else if (bytes == 1) {
            StdDumpDark("[%08x][      %02x]", offset, (u8) value);
        } else if (bytes == 2) {
            StdDumpDark("[%08x][    %04x]", offset, (u16) value);
        } else {
            Assert(!"Invalid number of bytes");
        }
    }
};

class ADebugger {
public:
    ADebugger();
    virtual ~ADebugger();

    void            Initialize      (void);
    void            PreExecute      (Processor *cpu, const Instruction *inst);
    void            PostExecute     (Processor *cpu, const Instruction *inst);
    void            ProcessPreRun   (const Process *proc, Processor *cpu);
    void            MemRead         (const Processor *cpu, u32 addr, u32 nBytes, cpbyte data);
    void            MemWrite        (const Processor *cpu, u32 addr, u32 nBytes, cpbyte data);
    Tracer *        GetTracer       (void) { return &m_tracer; }
    void            BreakOnNextInst (const std::string &info) { m_breakInfo = info; m_breakNext = true; }
    void            PrintContext    (void) const;

private:
    std::string     GetCommandString();
    void            TraceInstruction();
	
    /*
     * if return value = true, continue execution;
     * else, get another command
     */
    bool            ParseExecCommand(const std::string &cmd);
    void            CheckBreakpoints();
    void            SaveBreakpoints();
    void            LoadBreakpoints();
private:
    /*
     * Command table
     */
    void            DumpPrevInst    (void) const;
    void            DumpInstruction (void) const;
    void            DumpCPU         (void) const;
    void            DumpMemAccesses (void) const;
    void            DumpStack       (int nItems);
    void            DumpMem         ();
    void            DumpSection     (u32 address);
    void            DumpPage        (u32 address);
    void            ListBreakpoints (void);
    void            SetBreakpoint   (u32 addr);
    void            RemoveBreakpoint(u32 addr);     /* if addr == 0, remove all breakpoints */
private:
    const Processor *   m_currCpuPtr;
    const Instruction * m_currInstPtr;
    DebuggerState       m_state;
    std::set<u32>       m_breakpoints;
    std::set<u32>       m_pageBps;
    std::set<u32>       m_memBps;
    DataStream *        m_input;
    Instruction         m_lastInst;
    u32                 m_lastEip;
    i64                 m_instCount;
    bool                m_saveBreakpoints;
    bool                m_breakNext;
    std::string         m_breakInfo;
    
    static const int    MemBufferSize = 0x1000;
    int                 m_memReadCount;
    int                 m_memWriteCount;
    MemAccessInfo       m_memReads[MemBufferSize];
    MemAccessInfo       m_memWrites[MemBufferSize];

    Tracer              m_tracer;
};

extern ADebugger     LxDebugger;

#endif // __LOCHSDBG_DEBUGGER_H__
