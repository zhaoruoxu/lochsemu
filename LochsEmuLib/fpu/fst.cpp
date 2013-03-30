#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Fpu_Fstcw_D9_7(const Instruction *inst)
{
    // FSTCW m2byte
    u32 offset = Offset32(inst->Main.Argument1);
    u16 ctrl;
    FPU()->RestoreContext();
    __asm fstcw     ctrl;
    FPU()->SaveContext();
    MemWrite16(offset, ctrl, LX_REG_DS);
}

void Processor::Fpu_Fstsw_DFE0(const Instruction *inst)
{
    // FSTSW AX
    u16 status;
    FPU()->RestoreContext();
    __asm {
        fstsw ax
        mov status, ax
    }
    FPU()->SaveContext();
    AX = status;
}

void Processor::Fpu_Fst64fp_DD_2(const Instruction *inst)
{
    // FST m64fp
    u32 offset = Offset32(inst->Main.Argument1);
    u64 val;
    FPU()->RestoreContext();
    __asm fst   val;
    FPU()->SaveContext();
    WriteOperand64(inst, inst->Main.Argument1, offset, val);
}

void Processor::Fpu_Fstp64fp_DD_3(const Instruction *inst)
{
    // FSTP m64fp
    u32 offset = Offset32(inst->Main.Argument1);
    u64 val;
    FPU()->RestoreContext();
    __asm fstp  val;
    FPU()->SaveContext();
    WriteOperand64(inst, inst->Main.Argument1, offset, val);
}

void Processor::Fpu_Fstsw_DD_7(const Instruction *inst)
{
    // FSTSW m2byte
    u32 offset = Offset32(inst->Main.Argument1);
    u16 status;
    FPU()->RestoreContext();
    __asm fstsw     status;
    FPU()->SaveContext();
    MemWrite16(offset, status);
}

void Processor::Fpu_Fstp_DDD8(const Instruction *inst)
{
    // FSTP ST(i)
    int i = inst->Aux.opcode - 0xd8;
    FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fstp st(0); 
            break;
        case 1: __asm fstp st(1); 
            break;
        case 2: __asm fstp st(2); 
            break;
        case 3: __asm fstp st(3); 
            break;
        case 4: __asm fstp st(4); 
            break;
        case 5: __asm fstp st(5); 
            break;
        case 6: __asm fstp st(6); 
            break;
        case 7: __asm fstp st(7); 
            break;
        default: Assert(0);
    }
    FPU()->SaveContext();
}

void Processor::Fpu_Fistp32_DB_3(const Instruction *inst)
{
	// FISTP m32int
	u32 offset = Offset32(inst->Main.Argument1);
	u32 val;
	FPU()->RestoreContext();
	__asm fistp val;
	FPU()->SaveContext();
	WriteOperand32(inst, inst->Main.Argument1, offset, val);
}

void Processor::Fpu_Fistp64int_DF_7(const Instruction *inst)
{
    // FISTP m64int
    u32 offset = Offset32(inst->Main.Argument1);
    u64 val;
    FPU()->RestoreContext();
    __asm fistp val;
    FPU()->SaveContext();
    WriteOperand64(inst, inst->Main.Argument1, offset, val);
}

END_NAMESPACE_LOCHSEMU()