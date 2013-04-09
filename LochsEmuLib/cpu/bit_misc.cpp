#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()


void Processor::Bt_0FA3(const Instruction *inst)
{
    // BT r/m16/32, r16/32
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = ReadOperand16(inst, inst->Main.Argument2, NULL);
        u16 val = (val >> val2) & 1;
        CF = val;
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 val = (val1 >> val2) & 1;
        CF = val;
    }

}

void Processor::Btr_0FB3(const Instruction *inst)
{
    // BTR r/m16/32, r16/32
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        NOT_IMPLEMENTED();
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = ReadOperand32(inst, inst->Main.Argument2, NULL);
        u32 cf;
        __asm {
            mov     eax, val2
            btr     val1, eax
            jc      btr_cf1
            mov     cf, 0
            jmp     btr_end
btr_cf1:
            mov     cf, 1
btr_end:
        }
        CF = cf;
        WriteOperand32(inst, inst->Main.Argument1, offset, val1);
    }
}

void Processor::Bt_0FBA_4(const Instruction* inst)
{
    // BT r/m16/32, imm8
    u32 offset;
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 val2 = (u16) inst->Main.Inst.Immediat;
        u16 val = (val1 >> val2) & 1;
        CF = val;
    } else {
        u32 val1 = ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 val2 = (u32) inst->Main.Inst.Immediat;
        u32 val = (val1 >> val2) & 1;
        CF = val;
    }

}

void Processor::Bswap_0FC8(const Instruction *inst)
{
    // BSWAP r32
    u32 val1 = ReadOperand32(inst, inst->Main.Argument1, NULL);
    u32 val = val1 >> 24;
    u32 tmp = (val1 >> 8) & 0xff00;
    val = val | tmp;
    tmp = (val1 << 8) & 0xff0000;
    val = val | tmp;
    tmp = val1 << 24;
    val = val | tmp;
    WriteOperand32(inst, inst->Main.Argument1, NULL, val);
}

void Processor::Bsf_0FBC(const Instruction *inst)
{
    // BSF r16/32, r/m16/32
    if (inst->Main.Prefix.OperandSize) {
        u16 val = ReadOperand16(inst, inst->Main.Argument2, NULL);
        if (val == 0) {
            ZF = 1;
            return;
        }
        ZF = 0;
        u16 res;
        __asm {
            bsf ax, word ptr val
            mov word ptr res, ax
        }
        WriteOperand16(inst, inst->Main.Argument1, 0, res);
    } else {
        u32 val = ReadOperand32(inst, inst->Main.Argument2, NULL);
        if (val == 0) {
            ZF = 1;
            return;
        }
        ZF = 0;
        u32 res;
        __asm {
            bsf eax, val
            mov res, eax
        }
        WriteOperand32(inst, inst->Main.Argument1, 0, res);
    }
}

void Processor::Bsr_0FBD(const Instruction *inst)
{
    // BSR r16/32, r/m16/32
    if (inst->Main.Prefix.OperandSize) {
        u16 val = ReadOperand16(inst, inst->Main.Argument2, NULL);
        if (val == 0) {
            ZF = 1;
            return;
        }
        ZF = 0;
        u16 res;
        __asm {
            bsr ax, word ptr val
            mov word ptr res, ax
        }
        WriteOperand16(inst, inst->Main.Argument1, 0, res);
    } else {
        u32 val = ReadOperand32(inst, inst->Main.Argument2, NULL);
        if (val == 0) {
            ZF = 1;
            return;
        }
        ZF = 0;
        u32 res;
        __asm {
            bsr eax, val
            mov res, eax
        }
        WriteOperand32(inst, inst->Main.Argument1, 0, res);
    }
}

END_NAMESPACE_LOCHSEMU()