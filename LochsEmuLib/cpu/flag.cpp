#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Cmc_F5(const Instruction *inst)
{
    /**
     * CLD
     */
    CF = 1-CF;
    
}

void Processor::Clc_F8(const Instruction *inst)
{
    /**
     * CLD
     */
    CF = 0;
    
}
void Processor::Stc_F9(const Instruction *inst)
{
    /**
     * CLD
     */
    CF = 1;
    
}

void Processor::Cli_FA(const Instruction *inst)
{
    // CLI
    LxWarning("CLI is not implemented\n");
    IF = 0;
    
}

void Processor::Sti_FB(const Instruction *inst)
{
    // STI
    LxWarning("STI is not implemented\n");
    IF = 1;
    
}

void Processor::Cld_FC(const Instruction *inst)
{
    /**
     * CLD
     */
    DF = 0;
    
}

void Processor::Std_FD(const Instruction *inst)
{
    /**
     * STD
     */
    DF = 1;
    
}


END_NAMESPACE_LOCHSEMU()