#include "stdafx.h"
#include "disassembler.h"

#include "processor.h"
#include "instruction.h"
#include "memory.h"

Disassembler::Disassembler()
{
    m_ptrChangeHandler  = nullptr;
    m_dataUpdateHandler = nullptr;
}

Disassembler::~Disassembler()
{

}

void Disassembler::OnPreExecute( const Processor *cpu, const Instruction *inst )
{
    u32 eip = cpu->EIP;
    if (m_instMap.find(eip) == m_instMap.end()) {
        LxDebug("Disassembling...\n");
        RecursiveDisassemble(cpu, eip, cpu->Mem->GetSection(eip));
        LxDebug("Disassemble complete, instMap size = %d\n", m_instMap.size());
        //if (m_instDisasmHandler) m_instDisasmHandler(m_instVecTemp);
        if (m_dataUpdateHandler) m_dataUpdateHandler(&m_instMap);
    }
    if (m_ptrChangeHandler) m_ptrChangeHandler(eip);
}

void Disassembler::RecursiveDisassemble( const Processor *cpu, u32 eip, const Section *sec )
{
    Assert(sec);

    while (true) {
        if (m_instMap.find(eip) != m_instMap.end()) return; // already disassembled

        InstPtr inst(new Instruction);
        LxDecode(cpu->Mem->GetRawData(eip), inst.get(), eip);
        m_instMap[eip] = Inst(inst, eip);

        u32 opcode = inst->Main.Inst.Opcode;
        if (opcode == 0xc3 || opcode == 0xcb || opcode == 0xc2 || opcode == 0xca) {
            // 'ret' is met
            return;
        }

        u32 addrValue = (u32) inst->Main.Inst.AddrValue;
        if (addrValue != 0) {
            if (OPERAND_TYPE(inst->Main.Argument1.ArgType) == CONSTANT_TYPE) {
                if (cpu->Mem->GetSection(addrValue))
                    RecursiveDisassemble(cpu, addrValue, cpu->Mem->GetSection(addrValue));
            }
        }
        u32 nextEip = eip + inst->Length;
        const Section *nextSec = cpu->Mem->GetSection(nextEip);
        if (sec != nextSec) return;
        eip = nextEip;
    }
}

