#include "stdafx.h"
#include "processor.h"
#include "coprocessor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Fpu_Fldcw_D9_5(const Instruction *inst)
{
    // FLDCW
    u16 val = ReadOperand16(inst, inst->Main.Argument2, NULL);
    FPU()->RestoreContext();
    __asm fldcw   val;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fld_D9C0(const Instruction *inst)
{
    // FLD ST(i)
    int i = inst->Aux.opcode - 0xC0;
    FPU()->RestoreContext();
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
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fld1_D9E8(const Instruction *inst)
{
    // FLD1
    FPU()->RestoreContext();
    __asm fld1;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fldl2t_D9E9(const Instruction *inst)
{
    // FLDL2T
    FPU()->RestoreContext();
    __asm fldl2t;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fldl2e_D9EA(const Instruction *inst)
{
    // FLDL2E
    FPU()->RestoreContext();
    __asm fldl2e;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fldpi_D9EB(const Instruction *inst)
{
    // FLDPI
    FPU()->RestoreContext();
    __asm fldpi;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fldlg2_D9EC(const Instruction *inst)
{
    // FLDLG2
    FPU()->RestoreContext();
    __asm fldlg2;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fldln2_D9ED(const Instruction *inst)
{
    // FLDLN2
    FPU()->RestoreContext();
    __asm fldln2;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fldz_D9EE(const Instruction *inst)
{
    // FLDZ
    FPU()->RestoreContext();
    __asm fldz;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fild16_DF_0(const Instruction *inst)
{
	// FILD m16int
	u16 val = ReadOperand16(inst, inst->Main.Argument2, NULL);
	FPU()->RestoreContext();
	__asm fild val;
	FPU()->SaveContext();
	
}

void Processor::Fpu_Fild32_DB_0(const Instruction *inst)
{
    // FILD m32int
    u32 val = ReadOperand32(inst, inst->Main.Argument2, NULL);
    FPU()->RestoreContext();
    __asm fild  val;
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fild64_DF_5(const Instruction *inst)
{
	// FILD m64int
	u64 val = ReadOperand64(inst, inst->Main.Argument2, NULL);
	FPU()->RestoreContext();
	__asm fild val;
	FPU()->SaveContext();
	
}

void Processor::Fpu_Fld80_DB_5(const Instruction *inst)
{
    // FLD m80fp
    u32 offset = Offset32(inst->Main.Argument2);
    pbyte dataPtr = Mem->GetRawData(offset);
    FPU()->RestoreContext();
    __asm {
        mov     eax, dataPtr
        fld     tbyte ptr [eax]
    }
    FPU()->SaveContext();
    
}

void Processor::Fpu_Fld64fp_DD_0(const Instruction *inst)
{
    // FLD m64fp
    u64 val = ReadOperand64(inst, inst->Main.Argument2, NULL);
    FPU()->RestoreContext();
    __asm fld   val;
    FPU()->SaveContext();

    
}

END_NAMESPACE_LOCHSEMU()