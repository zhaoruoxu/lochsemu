#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Inc_40X(Processor *cpu, const Instruction *inst)
{
    /**
     * INC r16/32
     */
    if (inst->Main.Prefix.OperandSize) {
        u16 val1 = cpu->GP_Regs[inst->Main.Inst.Opcode - 0x40].X16;
        u16 r = val1 + 1;
        cpu->SetFlagsShift16(r);
        cpu->SetFlagOF16(r, PROMOTE_I32(val1) + PROMOTE_I32(1));
        cpu->GP_Regs[inst->Main.Inst.Opcode - 0x40].X16 = r;
    } else {
        u32 val1 = cpu->GP_Regs[inst->Main.Inst.Opcode - 0x40].X32;
        u32 r = val1 + 1;
        cpu->SetFlagsShift32(r); // ZF, PF, SF
        cpu->SetFlagOF32(r, PROMOTE_I64(val1) + PROMOTE_I64(1));
        cpu->GP_Regs[inst->Main.Inst.Opcode - 0x40].X32 = r;
    }
    RET_SUCCESS();
}

LxResult Inc_FE_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * INC r/m8
     */
    u32 offset;
    u8 val1 = cpu->ReadOperand8(inst, inst->Main.Argument1, &offset);
    u8 r = val1 + 1;
    cpu->SetFlagsShift8(r);
    cpu->SetFlagOF8(r, PROMOTE_I16(val1) + PROMOTE_I16(1));
    cpu->WriteOperand8(inst, inst->Main.Argument1, offset, r);
    RET_SUCCESS();
}

LxResult Inc_FF_ext0(Processor *cpu, const Instruction *inst)
{
    /**
     * INC r/m32
     * INC r/m16
     */
    if (inst->Main.Prefix.OperandSize) {
        u32 offset;
        u16 val1 = cpu->ReadOperand16(inst, inst->Main.Argument1, &offset);
        u16 r = val1 + 1;
        cpu->SetFlagsShift16(r);
        cpu->SetFlagOF16(r, PROMOTE_I32(val1) + PROMOTE_I32(1));
        cpu->WriteOperand16(inst, inst->Main.Argument1, offset, r);
    } else {
        u32 offset;
        u32 val1 = cpu->ReadOperand32(inst, inst->Main.Argument1, &offset);
        u32 r = val1 + 1;
        cpu->SetFlagsShift32(r);
        cpu->SetFlagOF32(r, PROMOTE_I64(val1) + PROMOTE_I64(1));
        cpu->WriteOperand32(inst, inst->Main.Argument1, offset, r);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()