#include "stdafx.h"
#include "processor.h"
#include "inst_table.h"

BEGIN_NAMESPACE_LOCHSEMU()

    
LxResult Cmovo_0F40(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovO rel8
     */
    if (cpu->OF == 1) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovno_0F41(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovNO rel8
     */
    if (cpu->OF == 0) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovb_0F42(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovB rel8
     */
    if (cpu->CF == 1) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovnb_0F43(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovNB rel8
     */
    if (cpu->CF == 0) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmove_0F44(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovE rel8
     */
    if (cpu->ZF == 1) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovne_0F45(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovNE rel8
     */
    if (cpu->ZF == 0) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovbe_0F46(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovBE rel8
     */
    if (cpu->CF == 1 || cpu->ZF == 1) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmova_0F47(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovA rel8
     */
    if (cpu->CF == 0 && cpu->ZF == 0) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovs_0F48(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovS rel8
     */
    if (cpu->SF == 1) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovns_0F49(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovNS rel8
     */
    if (cpu->SF == 0) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovp_0F4A(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovP rel8
     */
    if (cpu->PF == 1) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovnp_0F4B(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovPO rel8
     */
    if (cpu->PF == 0) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovl_0F4C(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovL rel8
     */
    if (cpu->SF != cpu->OF) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovge_0F4D(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovGE rel8
     */
    if (cpu->SF == cpu->OF) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovle_0F4E(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovLE rel8
     */
    if (cpu->ZF == 1 || cpu->SF != cpu->OF) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

LxResult Cmovg_0F4F(Processor *cpu, const Instruction *inst)
{
    /**
     * CmovG rel8
     */
    if (cpu->ZF == 0 && cpu->SF == cpu->OF) {
        return Mov_8B(cpu, inst);
    }
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()