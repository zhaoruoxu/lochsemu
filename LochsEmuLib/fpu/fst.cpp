#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()


LxResult Fpu_Fstcw_D9_7(Processor *cpu, const Instruction *inst)
{
    // FSTCW m2byte
    u32 offset = cpu->Offset32(inst->Main.Argument1);
    u16 ctrl;
    cpu->FPU()->RestoreContext();
    __asm fstcw     ctrl;
    cpu->FPU()->SaveContext();
    cpu->MemWrite16(offset, ctrl, LX_REG_DS);
    RET_SUCCESS();
}

LxResult Fpu_Fstsw_DFE0(Processor *cpu, const Instruction *inst)
{
    // FSTSW AX
    u16 status;
    cpu->FPU()->RestoreContext();
    __asm {
        fstsw ax
        mov status, ax
    }
    cpu->FPU()->SaveContext();
    cpu->AX = status;
    RET_SUCCESS();
}

LxResult Fpu_Fst64fp_DD_2(Processor *cpu, const Instruction *inst)
{
    // FST m64fp
    u32 offset = cpu->Offset32(inst->Main.Argument1);
    u64 val;
    cpu->FPU()->RestoreContext();
    __asm fst   val;
    cpu->FPU()->SaveContext();
    cpu->WriteOperand64(inst, inst->Main.Argument1, offset, val);
    RET_SUCCESS();
}

LxResult Fpu_Fstp64fp_DD_3(Processor *cpu, const Instruction *inst)
{
    // FSTP m64fp
    u32 offset = cpu->Offset32(inst->Main.Argument1);
    u64 val;
    cpu->FPU()->RestoreContext();
    __asm fstp  val;
    cpu->FPU()->SaveContext();
    cpu->WriteOperand64(inst, inst->Main.Argument1, offset, val);
    RET_SUCCESS();
}

LxResult Fpu_Fstsw_DD_7(Processor *cpu, const Instruction *inst)
{
    // FSTSW m2byte
    u32 offset = cpu->Offset32(inst->Main.Argument1);
    u16 status;
    cpu->FPU()->RestoreContext();
    __asm fstsw     status;
    cpu->FPU()->SaveContext();
    cpu->MemWrite16(offset, status);
    RET_SUCCESS();
}

LxResult Fpu_Fstp_DDD8(Processor *cpu, const Instruction *inst)
{
    // FSTP ST(i)
    int i = inst->Aux.opcode - 0xd8;
    cpu->FPU()->RestoreContext();
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
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fistp32_DB_3(Processor *cpu, const Instruction *inst)
{
	// FISTP m32int
	u32 offset = cpu->Offset32(inst->Main.Argument1);
	u32 val;
	cpu->FPU()->RestoreContext();
	__asm fistp val;
	cpu->FPU()->SaveContext();
	cpu->WriteOperand32(inst, inst->Main.Argument1, offset, val);
	RET_SUCCESS();
}

LxResult Fpu_Fistp64int_DF_7(Processor *cpu, const Instruction *inst)
{
    // FISTP m64int
    u32 offset = cpu->Offset32(inst->Main.Argument1);
    u64 val;
    cpu->FPU()->RestoreContext();
    __asm fistp val;
    cpu->FPU()->SaveContext();
    cpu->WriteOperand64(inst, inst->Main.Argument1, offset, val);

    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()