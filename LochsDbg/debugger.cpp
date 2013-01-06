#include "stdafx.h"
#include "debugger.h"
#include "console.h"
#include "processor.h"
#include "LochsDbg.h"
#include "config.h"
#include "diff.h"

ADebugger LxDebugger;

ADebugger::ADebugger()
{
    m_input         = NULL;
    m_state         = STATE_SINGLESTEP;
    m_instCount     = 0;
    m_lastEip       = 0xffffffff;
    m_memReadCount  = 0;
    m_memWriteCount = 0;
    m_currInstPtr   = NULL;
    m_currCpuPtr    = NULL;
}

ADebugger::~ADebugger()
{
    SAFE_DELETE(m_input);
}

void ADebugger::Initialize( void )
{
    m_input = new Console;
    std::string traceFile = LxGetModuleDirectory(g_module) + "lochsdbg_trace.txt";
    m_tracer.Initialize(traceFile.c_str());
    m_saveBreakpoints = g_config.GetInt("General", "SaveBreakpoints", 1) != 0;
    if (m_saveBreakpoints) {
        LoadBreakpoints();
    }
}

void ADebugger::ProcessPreRun( const Process *proc, Processor *cpu )
{
    if (g_config.GetInt("Breakpoints", "BreakOnEntryPoint", 1)) {
        m_breakpoints.insert(proc->GetEntryPoint());
    }
}

void ADebugger::PreExecute( Processor *cpu, const Instruction *inst )
{
    m_currCpuPtr    = cpu;
    m_currInstPtr   = inst;
    TraceInstruction();
    CheckBreakpoints();
    if (m_state == STATE_TERMINATED) {
        exit(0);
    } else if (m_state == STATE_SINGLESTEP) {
        PrintContext();

        std::string cmd;
        do {
            cmd = GetCommandString();
        } while (!ParseExecCommand(cmd));
    }  
    /*
     * update context
     */
    memcpy(&m_lastInst, inst, sizeof(Instruction));
    m_instCount++;
    m_lastEip = cpu->EIP;
    m_memReadCount = 0;
    m_memWriteCount = 0;
}


void ADebugger::PrintContext( void ) const
{
    if (m_instCount) {
        StdDumpLight("--------------------------------------------------------------\n");
        DumpPrevInst();
        DumpMemAccesses();
    }
    DumpCPU();
    DumpInstruction();
}

bool ADebugger::ParseExecCommand( const std::string &line )
{
    std::stringstream ss;
    ss << line;
    std::string cmd;
    ss >> cmd;
    if (cmd == "") {
        /*
         * Next step
         */
        return true;
    } else if (cmd == "bp") {
        /*
         * set or list breakpoint
         */
        if (ss.eof()) {
            ListBreakpoints();
        } else {
            u32 bp;
            ss >> std::hex >> bp;
            SetBreakpoint(bp);
        }
        return false;
    } else if (cmd == "bpr") {
        /*
         * remove breakpoint
         */
        if (ss.eof()) {
            RemoveBreakpoint(0);
            StdOut("All breakpoints removed\n");
        } else {
            u32 bp;
            ss >> std::hex >> bp;
            RemoveBreakpoint(bp);
            StdOut("Breakpoint removed: %08x\n", bp);
        }
        return false;
    }
    else if (cmd == "out") {
        /*
         * Step out in Diff
         */
        LxDiff.StepOut();
        m_state = STATE_RUNNING;
        return true;
    }
    else if (cmd == "ref") {
        /*
         * Print reference registers
         */
        LxDiff.PrintRefRegisters();
        return false;
    }
    else if (cmd == "r") {
        /*
         * continuous running
         */
        m_state = STATE_RUNNING;
        return true;
    } else if (cmd == "q") {
        ExitProcess(0);
        m_state = STATE_TERMINATED;
        return true;
    } else if (cmd == "stack") {
        if (ss.eof()) {
            DumpStack(g_config.GetInt("Display", "DefaultStackItems", 10));
        } else {
            int n;
            ss >> n;
            DumpStack(n);
        }
        return false;
    } else if (cmd == "mem") {
        DumpMem();
        return false;
    } else if (cmd == "sec") {
        u32 addr;
        ss >> std::hex >> addr;
        DumpSection(addr);
        return false;
    } else if (cmd == "page") {
        u32 addr;
        ss >> std::hex >> addr;
        DumpPage(addr);
        return false;
    } else if (cmd == "trace") {
        if (m_tracer.Enabled()) {
            StdOut("Trace disabled\n");
            m_tracer.Enable(false);
        } else {
            StdOut("Trace enabled\n");
            m_tracer.Enable(true);
        }
        return false;
	}
    
    else {
        /*
         * Invalid command
         */
        StdOut("Invalid command: %s\n", cmd.c_str());
        return false;
    }
}


void ADebugger::PostExecute( Processor *cpu, const Instruction *inst )
{
    //m_currCpuPtr    = cpu;
    //m_currInstPtr   = inst;
    //CheckBreakpoints();
    //if (m_state == STATE_TERMINATED) {
    //    exit(0);
    //} else if (m_state == STATE_SINGLESTEP) {
    //    PrintContext();

    //    std::string cmd;
    //    do {
    //        cmd = GetCommandString();
    //    } while (!ParseExecCommand(cmd));
    //}  
    ///*
    // * update context
    // */
    //memcpy(&m_lastInst, inst, sizeof(Instruction));
    //m_instCount++;
    //m_lastEip = cpu->EIP;
    //m_memReadCount = 0;
    //m_memWriteCount = 0;
}

void ADebugger::CheckBreakpoints()
{
    u32 eip = m_currCpuPtr->EIP;
    if (m_breakpoints.find(eip) != m_breakpoints.end()) {
        StdOut("Breakpoint hit: %08x\n", eip);
        m_state = STATE_SINGLESTEP;
    }
    if (m_breakNext) {
        StdOut("Breakpoint hit: %08x, info: %s\n", eip, m_breakInfo.c_str());
        m_breakNext = false;
        m_state = STATE_SINGLESTEP;
    }
}


std::string ADebugger::GetCommandString()
{
    StdDumpLight("%08x %s>", m_currCpuPtr->EIP, LxDiff.IsSynced() ? "SYNC" : "NSYNC");
    std::string line = m_input->ReadLine();
    //trim(line);
    return line;
}

void ADebugger::MemRead( const Processor *cpu, u32 addr, u32 nBytes, cpbyte data )
{
    Assert(nBytes <= 16);
    if (m_memReadCount >= MemBufferSize) return; // Too many memory access, ignore
    if (nBytes == 16)
        m_memReads[m_memReadCount] = MemAccessInfo(addr, (u128p) data, nBytes);
    else 
        m_memReads[m_memReadCount] = MemAccessInfo(addr, (u64p) data, nBytes);
    m_memReadCount++;
    if (m_memReadCount >= MemBufferSize) {
        StdError("Too many memory reads(%d)!!!\n", m_memReadCount);
    }
}

void ADebugger::MemWrite( const Processor *cpu, u32 addr, u32 nBytes, cpbyte data )
{
    Assert(nBytes <= 16);
    if (m_memWriteCount >= MemBufferSize) return; // Too many memory access, ignore
    if (nBytes == 16)
        m_memWrites[m_memWriteCount] = MemAccessInfo(addr, (u128p) data, nBytes);
    else 
        m_memWrites[m_memWriteCount] = MemAccessInfo(addr, (u64p) data, nBytes);
    m_memWriteCount++;
    if (m_memWriteCount >= MemBufferSize) {
        StdError("Too many memory writes(%d)!!!\n", m_memWriteCount);
    }
}

void ADebugger::TraceInstruction()
{
    if (!m_tracer.Enabled()) return;
    if (m_memReadCount > 0) {
        m_tracer.Trace("\t\t\tMR ");
        for (int i = 0; i < m_memReadCount; i++) {
            m_tracer.Trace("[%08x][%d]%08x ", m_memReads[i].offset, m_memReads[i].bytes, m_memReads[i].value);
        }
        m_tracer.Trace("\n");
    }
    if (m_memWriteCount > 0) {
        m_tracer.Trace("\t\t\tMW ");
        for (int i = 0; i < m_memWriteCount; i++) {
            m_tracer.Trace("[%08x][%d]%08x ", m_memWrites[i].offset, m_memWrites[i].bytes, m_memWrites[i].value);
        }
        m_tracer.Trace("\n");
    }
    m_tracer.Trace("\t\t\tEAX[%08x] ECX[%08x] EDX[%08x] EBX[%08x] ESP[%08x] EBP[%08x] ESI[%08x] EDI[%08x]\n",
        m_currCpuPtr->EAX, m_currCpuPtr->ECX, m_currCpuPtr->EDX, m_currCpuPtr->EBX,
        m_currCpuPtr->ESP, m_currCpuPtr->EBP, m_currCpuPtr->ESI, m_currCpuPtr->EDI);
    m_tracer.Trace("[%08x]  %s\n", m_currCpuPtr->EIP, m_currInstPtr->Main.CompleteInstr);
}

void ADebugger::SaveBreakpoints()
{
    const uint numBps = m_breakpoints.size();
    g_config.SetInt("Breakpoints", "Number", numBps);
    char buf[64];
    std::set<u32>::const_iterator iter = m_breakpoints.begin();
    uint n = 0;
    for (; iter != m_breakpoints.end(); iter++) {
        sprintf(buf, "Breakpoint%d", n);
        g_config.SetUint("Breakpoints", buf, *iter);
        n++;
    }
}

void ADebugger::LoadBreakpoints()
{
    const uint numBps = g_config.GetInt("Breakpoints", "Number", 0);
    char buf[64];
    for (uint i = 0; i < numBps; i++) {
        sprintf(buf, "Breakpoint%d", i);
        u32 bp = g_config.GetUint("Breakpoints", buf, 0);
        if (bp == 0) {
            StdOut("Invalid breakpoint[%d] in config file\n", i);
            continue;
        }
        m_breakpoints.insert(bp);
    }
    StdOut("Loaded %d breakpoints\n", m_breakpoints.size());
}

