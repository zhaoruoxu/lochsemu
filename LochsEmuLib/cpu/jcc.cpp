#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

static void JumpRel8(Processor *cpu, const Instruction *inst)
{
    u32 rel = SIGN_EXTEND(8, 32, inst->Aux.op1.immediate);
    cpu->EIP += rel;
}

static void JumpRel32(Processor *cpu, const Instruction *inst)
{
    u32 rel = inst->Aux.op1.immediate;
    cpu->EIP += rel;
}

LxResult Jo_70(Processor *cpu, const Instruction *inst)
{
    /**
     * JO rel8
     */
    if (cpu->OF == 1) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jno_71(Processor *cpu, const Instruction *inst)
{
    /**
     * JNO rel8
     */
    if (cpu->OF == 0) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jb_72(Processor *cpu, const Instruction *inst)
{
    /**
     * JB rel8
     */
    if (cpu->CF == 1) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jnb_73(Processor *cpu, const Instruction *inst)
{
    /**
     * JNB rel8
     */
    if (cpu->CF == 0) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Je_74(Processor *cpu, const Instruction *inst)
{
    /**
     * JE rel8
     */
    if (cpu->ZF == 1) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jne_75(Processor *cpu, const Instruction *inst)
{
    /**
     * JNE rel8
     */
    if (cpu->ZF == 0) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jbe_76(Processor *cpu, const Instruction *inst)
{
    /**
     * JBE rel8
     */
    if (cpu->CF == 1 || cpu->ZF == 1) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Ja_77(Processor *cpu, const Instruction *inst)
{
    /**
     * JA rel8
     */
    if (cpu->CF == 0 && cpu->ZF == 0) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Js_78(Processor *cpu, const Instruction *inst)
{
    /**
     * JS rel8
     */
    if (cpu->SF == 1) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jns_79(Processor *cpu, const Instruction *inst)
{
    /**
     * JNS rel8
     */
    if (cpu->SF == 0) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jp_7A(Processor *cpu, const Instruction *inst)
{
    /**
     * JP rel8
     */
    if (cpu->PF == 1) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jnp_7B(Processor *cpu, const Instruction *inst)
{
    /**
     * JPO rel8
     */
    if (cpu->PF == 0) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jl_7C(Processor *cpu, const Instruction *inst)
{
    /**
     * JL rel8
     */
    if (cpu->SF != cpu->OF) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jge_7D(Processor *cpu, const Instruction *inst)
{
    /**
     * JGE rel8
     */
    if (cpu->SF == cpu->OF) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jle_7E(Processor *cpu, const Instruction *inst)
{
    /**
     * JLE rel8
     */
    if (cpu->ZF == 1 || cpu->SF != cpu->OF) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jg_7F(Processor *cpu, const Instruction *inst)
{
    /**
     * JG rel8
     */
    if (cpu->ZF == 0 && cpu->SF == cpu->OF) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jecxz_E3(Processor *cpu, const Instruction *inst)
{
    /**
     * JECXZ rel8
     */
    if (inst->Main.Prefix.OperandSize) {
        return LX_RESULT_NOT_IMPLEMENTED;
    }
    if (cpu->ECX == 0) {
        JumpRel8(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jo_0F80(Processor *cpu, const Instruction *inst)
{
    /**
     * JO rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    if (cpu->OF == 1) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jno_0F81(Processor *cpu, const Instruction *inst)
{
    /**
     * JNO rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    if (cpu->OF == 0) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jc_0F82(Processor *cpu, const Instruction *inst)
{
    /**
     * JC rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    if (cpu->CF == 1) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jnb_0F83(Processor *cpu, const Instruction *inst)
{
    /**
     * JNB rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    if (cpu->CF == 0) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jz_0F84(Processor *cpu, const Instruction *inst)
{
    /**
     * JZ rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    if (cpu->ZF == 1) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jnz_0F85(Processor *cpu, const Instruction *inst)
{
    /**
     * JNZ rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    if (cpu->ZF == 0) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jna_0F86(Processor *cpu, const Instruction *inst)
{
    /**
     * JNA rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    if (cpu->CF == 1 || cpu->ZF == 1) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Ja_0F87(Processor *cpu, const Instruction *inst)
{
    /**
     * JA rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    if (cpu->ZF == 0 && cpu->CF == 0) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Js_0F88(Processor *cpu, const Instruction *inst)
{
    /**
     * JS rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    if (cpu->SF == 1) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jns_0F89(Processor *cpu, const Instruction *inst)
{
    /**
     * JNS rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    if (cpu->SF == 0) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jp_0F8A(Processor *cpu, const Instruction *inst)
{
    /**
     * JP rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    if (cpu->PF == 1) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jpo_0F8B(Processor *cpu, const Instruction *inst)
{
    /**
     * JPO rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    if (cpu->PF == 0) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jl_0F8C(Processor *cpu, const Instruction *inst)
{
    /**
     * JL rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    if (cpu->SF != cpu->OF) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jnl_0F8D(Processor *cpu, const Instruction *inst)
{
    /**
     * JNL rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    if (cpu->SF == cpu->OF) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jng_0F8E(Processor *cpu, const Instruction *inst)
{
    /**
     * JNG rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();

    if (cpu->ZF == 1 || cpu->SF != cpu->OF) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Jnle_0F8F(Processor *cpu, const Instruction *inst)
{
    /**
     * JNLE rel32
     */
    if (inst->Main.Prefix.OperandSize) RET_NOT_IMPLEMENTED();
    if (cpu->ZF == 0 && cpu->SF == cpu->OF) {
        JumpRel32(cpu, inst);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()