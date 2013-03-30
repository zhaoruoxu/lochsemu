#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

    
void Processor::Cmovo_0F40(const Instruction *inst)
{
    /**
     * CmovO rel8
     */
    if (OF == 1) {
        Mov_8B(inst);
    }
}

void Processor::Cmovno_0F41(const Instruction *inst)
{
    /**
     * CmovNO rel8
     */
    if (OF == 0) {
        Mov_8B(inst);
    }
}

void Processor::Cmovb_0F42(const Instruction *inst)
{
    /**
     * CmovB rel8
     */
    if (CF == 1) {
        Mov_8B(inst);
    }
}

void Processor::Cmovnb_0F43(const Instruction *inst)
{
    /**
     * CmovNB rel8
     */
    if (CF == 0) {
        Mov_8B(inst);
    }
}

void Processor::Cmove_0F44(const Instruction *inst)
{
    /**
     * CmovE rel8
     */
    if (ZF == 1) {
        Mov_8B(inst);
    }
}

void Processor::Cmovne_0F45(const Instruction *inst)
{
    /**
     * CmovNE rel8
     */
    if (ZF == 0) {
        Mov_8B(inst);
    }
}

void Processor::Cmovbe_0F46(const Instruction *inst)
{
    /**
     * CmovBE rel8
     */
    if (CF == 1 || ZF == 1) {
        Mov_8B(inst);
    }
}

void Processor::Cmova_0F47(const Instruction *inst)
{
    /**
     * CmovA rel8
     */
    if (CF == 0 && ZF == 0) {
        Mov_8B(inst);
    }
}

void Processor::Cmovs_0F48(const Instruction *inst)
{
    /**
     * CmovS rel8
     */
    if (SF == 1) {
        Mov_8B(inst);
    }
}

void Processor::Cmovns_0F49(const Instruction *inst)
{
    /**
     * CmovNS rel8
     */
    if (SF == 0) {
        Mov_8B(inst);
    }
}

void Processor::Cmovp_0F4A(const Instruction *inst)
{
    /**
     * CmovP rel8
     */
    if (PF == 1) {
        Mov_8B(inst);
    }
}

void Processor::Cmovnp_0F4B(const Instruction *inst)
{
    /**
     * CmovPO rel8
     */
    if (PF == 0) {
        Mov_8B(inst);
    }
}

void Processor::Cmovl_0F4C(const Instruction *inst)
{
    /**
     * CmovL rel8
     */
    if (SF != OF) {
        Mov_8B(inst);
    }
}

void Processor::Cmovge_0F4D(const Instruction *inst)
{
    /**
     * CmovGE rel8
     */
    if (SF == OF) {
        Mov_8B(inst);
    }
}

void Processor::Cmovle_0F4E(const Instruction *inst)
{
    /**
     * CmovLE rel8
     */
    if (ZF == 1 || SF != OF) {
        Mov_8B(inst);
    }
}

void Processor::Cmovg_0F4F(const Instruction *inst)
{
    /**
     * CmovG rel8
     */
    if (ZF == 0 && SF == OF) {
        Mov_8B(inst);
    }
}

END_NAMESPACE_LOCHSEMU()