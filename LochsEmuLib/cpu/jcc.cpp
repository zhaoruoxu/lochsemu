#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::JumpRel32(const Instruction *inst)
{
    u32 rel = inst->Aux.op1.immediate;
    EIP += rel;
}

void Processor::Jo_70(const Instruction *inst)
{
    /**
     * JO rel8
     */
    if (OF == 1) {
        JumpRel8(inst);
    }
    
}

void Processor::Jno_71(const Instruction *inst)
{
    /**
     * JNO rel8
     */
    if (OF == 0) {
        JumpRel8(inst);
    }
    
}

void Processor::Jb_72(const Instruction *inst)
{
    /**
     * JB rel8
     */
    if (CF == 1) {
        JumpRel8(inst);
    }
    
}

void Processor::Jnb_73(const Instruction *inst)
{
    /**
     * JNB rel8
     */
    if (CF == 0) {
        JumpRel8(inst);
    }
    
}

void Processor::Je_74(const Instruction *inst)
{
    /**
     * JE rel8
     */
    if (ZF == 1) {
        JumpRel8(inst);
    }
    
}

void Processor::Jne_75(const Instruction *inst)
{
    /**
     * JNE rel8
     */
    if (ZF == 0) {
        JumpRel8(inst);
    }
    
}

void Processor::Jbe_76(const Instruction *inst)
{
    /**
     * JBE rel8
     */
    if (CF == 1 || ZF == 1) {
        JumpRel8(inst);
    }
    
}

void Processor::Ja_77(const Instruction *inst)
{
    /**
     * JA rel8
     */
    if (CF == 0 && ZF == 0) {
        JumpRel8(inst);
    }
    
}

void Processor::Js_78(const Instruction *inst)
{
    /**
     * JS rel8
     */
    if (SF == 1) {
        JumpRel8(inst);
    }
    
}

void Processor::Jns_79(const Instruction *inst)
{
    /**
     * JNS rel8
     */
    if (SF == 0) {
        JumpRel8(inst);
    }
    
}

void Processor::Jp_7A(const Instruction *inst)
{
    /**
     * JP rel8
     */
    if (PF == 1) {
        JumpRel8(inst);
    }
    
}

void Processor::Jnp_7B(const Instruction *inst)
{
    /**
     * JPO rel8
     */
    if (PF == 0) {
        JumpRel8(inst);
    }
    
}

void Processor::Jl_7C(const Instruction *inst)
{
    /**
     * JL rel8
     */
    if (SF != OF) {
        JumpRel8(inst);
    }
    
}

void Processor::Jge_7D(const Instruction *inst)
{
    /**
     * JGE rel8
     */
    if (SF == OF) {
        JumpRel8(inst);
    }
    
}

void Processor::Jle_7E(const Instruction *inst)
{
    /**
     * JLE rel8
     */
    if (ZF == 1 || SF != OF) {
        JumpRel8(inst);
    }
    
}

void Processor::Jg_7F(const Instruction *inst)
{
    /**
     * JG rel8
     */
    if (ZF == 0 && SF == OF) {
        JumpRel8(inst);
    }
    
}

void Processor::Jecxz_E3(const Instruction *inst)
{
    /**
     * JECXZ rel8
     */
    if (inst->Main.Prefix.OperandSize) {
        NOT_IMPLEMENTED();
    }
    if (ECX == 0) {
        JumpRel8(inst);
    }
    
}

void Processor::Jo_0F80(const Instruction *inst)
{
    /**
     * JO rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    if (OF == 1) {
        JumpRel32(inst);
    }
    
}

void Processor::Jno_0F81(const Instruction *inst)
{
    /**
     * JNO rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    if (OF == 0) {
        JumpRel32(inst);
    }
    
}

void Processor::Jc_0F82(const Instruction *inst)
{
    /**
     * JC rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    if (CF == 1) {
        JumpRel32(inst);
    }
    
}

void Processor::Jnb_0F83(const Instruction *inst)
{
    /**
     * JNB rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    if (CF == 0) {
        JumpRel32(inst);
    }
    
}

void Processor::Jz_0F84(const Instruction *inst)
{
    /**
     * JZ rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    if (ZF == 1) {
        JumpRel32(inst);
    }
    
}

void Processor::Jnz_0F85(const Instruction *inst)
{
    /**
     * JNZ rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    if (ZF == 0) {
        JumpRel32(inst);
    }
    
}

void Processor::Jna_0F86(const Instruction *inst)
{
    /**
     * JNA rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    if (CF == 1 || ZF == 1) {
        JumpRel32(inst);
    }
    
}

void Processor::Ja_0F87(const Instruction *inst)
{
    /**
     * JA rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    if (ZF == 0 && CF == 0) {
        JumpRel32(inst);
    }
    
}

void Processor::Js_0F88(const Instruction *inst)
{
    /**
     * JS rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    if (SF == 1) {
        JumpRel32(inst);
    }
    
}

void Processor::Jns_0F89(const Instruction *inst)
{
    /**
     * JNS rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    if (SF == 0) {
        JumpRel32(inst);
    }
    
}

void Processor::Jp_0F8A(const Instruction *inst)
{
    /**
     * JP rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    if (PF == 1) {
        JumpRel32(inst);
    }
    
}

void Processor::Jpo_0F8B(const Instruction *inst)
{
    /**
     * JPO rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    if (PF == 0) {
        JumpRel32(inst);
    }
    
}

void Processor::Jl_0F8C(const Instruction *inst)
{
    /**
     * JL rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    if (SF != OF) {
        JumpRel32(inst);
    }
    
}

void Processor::Jnl_0F8D(const Instruction *inst)
{
    /**
     * JNL rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    if (SF == OF) {
        JumpRel32(inst);
    }
    
}

void Processor::Jng_0F8E(const Instruction *inst)
{
    /**
     * JNG rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();

    if (ZF == 1 || SF != OF) {
        JumpRel32(inst);
    }
    
}

void Processor::Jnle_0F8F(const Instruction *inst)
{
    /**
     * JNLE rel32
     */
    if (inst->Main.Prefix.OperandSize) NOT_IMPLEMENTED();
    if (ZF == 0 && SF == OF) {
        JumpRel32(inst);
    }
    
}

END_NAMESPACE_LOCHSEMU()