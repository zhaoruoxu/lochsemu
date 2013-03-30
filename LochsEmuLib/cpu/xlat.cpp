#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

void Processor::Xlat_D7(const Instruction *inst)
{
    /**
     * xlat: AL¡ûDS:[(E)BX + unsigned AL]
     */
    if (inst->Main.Prefix.AddressSize) NOT_IMPLEMENTED();

    u32 offset = EBX + AL;
	AL = MemRead8(offset);
    
}

END_NAMESPACE_LOCHSEMU()