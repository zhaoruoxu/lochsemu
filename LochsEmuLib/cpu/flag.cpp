#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Cmc_F5(Processor *cpu, const Instruction *inst)
{
    /**
     * CLD
     */
    cpu->CF = 1-cpu->CF;
    RET_SUCCESS();
}

LxResult Clc_F8(Processor *cpu, const Instruction *inst)
{
    /**
     * CLD
     */
    cpu->CF = 0;
    RET_SUCCESS();
}
LxResult Stc_F9(Processor *cpu, const Instruction *inst)
{
    /**
     * CLD
     */
    cpu->CF = 1;
    RET_SUCCESS();
}

LxResult Cli_FA(Processor *cpu, const Instruction *inst)
{
    // CLI
    LxWarning("CLI is not implemented\n");
    cpu->IF = 0;
    RET_SUCCESS();
}

LxResult Sti_FB(Processor *cpu, const Instruction *inst)
{
    // STI
    LxWarning("STI is not implemented\n");
    cpu->IF = 1;
    RET_SUCCESS();
}

LxResult Cld_FC(Processor *cpu, const Instruction *inst)
{
    /**
     * CLD
     */
    cpu->DF = 0;
    RET_SUCCESS();
}

LxResult Std_FD(Processor *cpu, const Instruction *inst)
{
    /**
     * STD
     */
    cpu->DF = 1;
    RET_SUCCESS();
}


END_NAMESPACE_LOCHSEMU()