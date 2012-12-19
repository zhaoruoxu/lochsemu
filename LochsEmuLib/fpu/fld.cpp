#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Fpu_Fldcw_D9_5(Processor *cpu, const Instruction *inst)
{
    // FLDCW
    u16 val = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fldcw   val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fld_D9C0(Processor *cpu, const Instruction *inst)
{
    // FLD ST(i)
    int i = inst->Aux.opcode - 0xC0;
    cpu->FPU()->RestoreContext();
    switch (i) {
        case 0: __asm fld st(0); 
            break;
        case 1: __asm fld st(1); 
            break;
        case 2: __asm fld st(2); 
            break;
        case 3: __asm fld st(3); 
            break;
        case 4: __asm fld st(4); 
            break;
        case 5: __asm fld st(5); 
            break;
        case 6: __asm fld st(6); 
            break;
        case 7: __asm fld st(7); 
            break;
        default: Assert(0);
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fld1_D9E8(Processor *cpu, const Instruction *inst)
{
    // FLD1
    cpu->FPU()->RestoreContext();
    __asm fld1;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fldl2t_D9E9(Processor *cpu, const Instruction *inst)
{
    // FLDL2T
    cpu->FPU()->RestoreContext();
    __asm fldl2t;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fldl2e_D9EA(Processor *cpu, const Instruction *inst)
{
    // FLDL2E
    cpu->FPU()->RestoreContext();
    __asm fldl2e;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fldpi_D9EB(Processor *cpu, const Instruction *inst)
{
    // FLDPI
    cpu->FPU()->RestoreContext();
    __asm fldpi;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fldlg2_D9EC(Processor *cpu, const Instruction *inst)
{
    // FLDLG2
    cpu->FPU()->RestoreContext();
    __asm fldlg2;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fldln2_D9ED(Processor *cpu, const Instruction *inst)
{
    // FLDLN2
    cpu->FPU()->RestoreContext();
    __asm fldln2;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fldz_D9EE(Processor *cpu, const Instruction *inst)
{
    // FLDZ
    cpu->FPU()->RestoreContext();
    __asm fldz;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fild16_DF_0(Processor *cpu, const Instruction *inst)
{
	// FILD m16int
	u16 val = cpu->ReadOperand16(inst, inst->Main.Argument2, NULL);
	cpu->FPU()->RestoreContext();
	__asm fild val;
	cpu->FPU()->SaveContext();
	RET_SUCCESS();
}

LxResult Fpu_Fild32_DB_0(Processor *cpu, const Instruction *inst)
{
    // FILD m32int
    u32 val = cpu->ReadOperand32(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fild  val;
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fild64_DF_5(Processor *cpu, const Instruction *inst)
{
	// FILD m64int
	u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
	cpu->FPU()->RestoreContext();
	__asm fild val;
	cpu->FPU()->SaveContext();
	RET_SUCCESS();
}

LxResult Fpu_Fld80_DB_5(Processor *cpu, const Instruction *inst)
{
    // FLD m80fp
    u32 offset = cpu->Offset32(inst->Main.Argument2);
    pbyte dataPtr = cpu->Mem->GetRawData(offset);
    cpu->FPU()->RestoreContext();
    __asm {
        mov     eax, dataPtr
        fld     tbyte ptr [eax]
    }
    cpu->FPU()->SaveContext();
    RET_SUCCESS();
}

LxResult Fpu_Fld64fp_DD_0(Processor *cpu, const Instruction *inst)
{
    // FLD m64fp
    u64 val = cpu->ReadOperand64(inst, inst->Main.Argument2, NULL);
    cpu->FPU()->RestoreContext();
    __asm fld   val;
    cpu->FPU()->SaveContext();

    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()