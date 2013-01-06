#include "stdafx.h"
#include "debugger.h"
#include "processor.h"
#include "stack.h"
#include "winapi.h"


void ADebugger::DumpInstruction() const
{
    StdDumpDark("[%I64d][%08x][%02x]    ", m_instCount, m_currCpuPtr->EIP, 
        m_currInstPtr->Main.Inst.Opcode);
    StdDumpLight("%s", m_currInstPtr->Main.CompleteInstr);

    u32 target = 0;
    const char *mnemonic = m_currInstPtr->Main.Inst.Mnemonic;
    if (m_currInstPtr->Main.Inst.Opcode == 0xff) {
        // CALL or JMP r/m32
        if (strstr(mnemonic, "jmp") == mnemonic || strstr(mnemonic, "call") == mnemonic)  {
            target = m_currCpuPtr->ReadOperand32(m_currInstPtr, m_currInstPtr->Main.Argument1, NULL);
        }

    } else if (m_currInstPtr->Main.Inst.Opcode == 0xe8 && strstr(mnemonic, "call") == mnemonic) {
        // call rel32
        target = m_currCpuPtr->EIP + m_currInstPtr->Length + (u32) m_currInstPtr->Aux.op1.immediate;
        // decode instruction at target
        pbyte codePtr = m_currCpuPtr->GetCodePtr(target);
        Instruction inst;
        LxDecode(codePtr, &inst, target);

        if (inst.Main.Inst.Opcode == 0xff && 
            (strstr(inst.Main.Inst.Mnemonic, "jmp") == inst.Main.Inst.Mnemonic)) {
            target = m_currCpuPtr->ReadOperand32(&inst, inst.Main.Argument1, NULL);
        } else {
            target = 0;
        }
    }

    const ApiInfo *info = m_currCpuPtr->Proc()->GetApiInfoFromAddress(target);
    if (info) {
        StdDumpLight(" < %s:%s >", info->ModuleName.c_str(), info->FunctionName.c_str());
    }

    StdDumpLight("\n");

}


void ADebugger::DumpPrevInst() const
{
    StdDumpDark("[%I64d][%08x][%02x]    ", m_instCount-1, m_lastEip, m_lastInst.Main.Inst.Opcode);
    StdDumpDark("%s\n", m_lastInst.Main.CompleteInstr);
}

void ADebugger::DumpCPU() const
{
    StdDumpLight("    EAX[%08x] ECX[%08x] EDX[%08x] EBX[%08x]\n    ESP[%08x] EBP[%08x] ESI[%08x] EDI[%08x]\n",
        m_currCpuPtr->EAX, m_currCpuPtr->ECX, m_currCpuPtr->EDX, m_currCpuPtr->EBX, 
        m_currCpuPtr->ESP, m_currCpuPtr->EBP, m_currCpuPtr->ESI, m_currCpuPtr->EDI);
    StdDumpDark("    EFLAGS: ");
    if (m_currCpuPtr->CF) StdDumpDark("CF ");
    if (m_currCpuPtr->PF) StdDumpDark("PF ");
    if (m_currCpuPtr->AF) StdDumpDark("AF ");
    if (m_currCpuPtr->ZF) StdDumpDark("ZF ");
    if (m_currCpuPtr->SF) StdDumpDark("SF ");
    if (m_currCpuPtr->TF) StdDumpDark("TF ");
    if (m_currCpuPtr->IF) StdDumpDark("IF ");
    if (m_currCpuPtr->DF) StdDumpDark("DF ");
    if (m_currCpuPtr->OF) StdDumpDark("OF ");
    StdDumpDark("\n");
}


void ADebugger::DumpMemAccesses( void ) const
{
    if (m_memReadCount > 0) {
        StdDumpLight("    MR: ");
        for (int i = 0; i < m_memReadCount; i++) {
            m_memReads[i].Dump();
            StdDumpDark("  ");
        }
        StdDumpLight("\n");
    }
    if (m_memWriteCount > 0) {
        StdDumpLight("    MW: ");
        for (int i = 0; i < m_memWriteCount; i++) {
            m_memWrites[i].Dump();
            StdDumpDark("  ");
        }
        StdDumpLight("\n");
    }
}


void ADebugger::DumpStack( int nItems )
{
    if (nItems < 0 || nItems > g_config.GetInt("Display", "MaxStackItems", 1024)) {
        nItems = g_config.GetInt("Display", "DefaultStackItems", 10);
    }
    StdDumpDark("\tESP = %08x\n", m_currCpuPtr->ESP);
    for (int i = 0; i < nItems; i++) {
        u32 esp = m_currCpuPtr->ESP + 4 * i;
        if (esp > m_currCpuPtr->Thr()->GetStack()->Top()) {
            StdDumpDark("\t---TOP OF STACK---\n");
            break;
        }
        u32 val;
        V( m_currCpuPtr->Mem->Read32(esp, &val) );
        StdDumpLight("\t[%08x] = %08x\n", esp, val);
    }
}


void ADebugger::DumpMem()
{
    std::vector<SectionInfo> info = m_currCpuPtr->Mem->GetMemoryInfo();
    StdDumpDark("\tNo.   Range                 Size      Module Desc\n");
    for (uint i = 0; i < info.size(); i++) {
        StdDumpDark("\t%3d - ", i);
        StdDumpLight("[%08x-%08x]", info[i].base, info[i].base + info[i].size - 1);
        StdDumpDark("  %08x  %d %s\n", info[i].size, info[i].Module, info[i].Desc.c_str());
    }
}


void ADebugger::DumpSection( u32 address )
{
    static const LPCSTR PageState[] = {
        "FREE     ", "RESERVED ", "COMMITTED" 
    };
    if (!m_currCpuPtr->Mem->Contains(address)) {
        StdOut("Invalid address\n");
        return;
    }
    std::vector<PageInfo>    info = m_currCpuPtr->Mem->GetSectionInfo(address);
    StdDumpDark("\tNo.   Base      State      Protection\n");
    for (uint i = 0; i < info.size(); i++) {
        StdDumpLight("\t%03d - %08x  %s  ", i, info[i].address, PageState[info[i].Characristics]);
        std::string protect;
        switch (info[i].Protect & 0xff) {
            case PAGE_NOACCESS:
                protect = "NOACCESS"; break;
            case PAGE_READONLY:
                protect = "READONLY"; break;
            case PAGE_READWRITE:
                protect = "READWRITE"; break;
            case PAGE_WRITECOPY:
                protect = "WRITECOPY"; break;
            case PAGE_EXECUTE:
                protect = "EXECUTE"; break;
            case PAGE_EXECUTE_READ:
                protect = "EXECUTE_READ"; break;
            case PAGE_EXECUTE_READWRITE:
                protect = "EXECUTE_READWRITE"; break;
            case PAGE_EXECUTE_WRITECOPY:
                protect = "EXECUTE_WRITECOPY"; break;
            default:
                protect = "UNKNOWN";
        }
        StdDumpLight("%s\n", protect.c_str());
    }
}


void ADebugger::DumpPage( u32 address )
{
    u32 base = PAGE_HIGH(address);
    if (!m_currCpuPtr->Mem->Contains(base)) {
        StdOut("Invalid address\n");
        return;
    }
    pbyte raw = m_currCpuPtr->Mem->GetRawData(base);
    Assert(raw);
    for (uint i = 0; i < LX_PAGE_SIZE; i++) {
        uint offset = i % 16;
        if (offset == 0) {
            StdDumpDark("%08x  ", base + i);
        }
        StdDumpLight("%02x", *(raw + i));
        if ((i & 3) == 3) StdDumpDark(" | ");
        if (offset == 15) {
            char buf[17];
            memcpy(buf, raw + i - offset, 16);
            for (int j = 0; j < 16; j++) {
                if (buf[j] > 0x7f || buf[j] < 0x20)
                    buf[j] = '.';
            }
            buf[16] = '\0';
            StdDumpLight("%s\n", buf);
        }
    }
}   


void ADebugger::SetBreakpoint( u32 addr )
{
    StdOut("bp set: %08x\n", addr);
    m_breakpoints.insert(addr);
    if (m_saveBreakpoints) {
        SaveBreakpoints();
    }
}


void ADebugger::RemoveBreakpoint( u32 addr )
{
    if (addr == 0) {
        m_breakpoints.clear();
    } else {
        m_breakpoints.erase(addr);
    }
    if (m_saveBreakpoints) {
        SaveBreakpoints();
    }
}



void ADebugger::ListBreakpoints()
{
    StdDumpDark("    Total:\t");
    StdDumpLight("%d\n", m_breakpoints.size());
    std::set<u32>::const_iterator iter = m_breakpoints.begin();
    for (; iter != m_breakpoints.end(); iter++) {
        StdDumpDark("    Breakpoint:\t");
        StdDumpLight("%08x\n", *iter);
    }
}