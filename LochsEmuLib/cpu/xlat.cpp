#include "stdafx.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()

LxResult Xlat_D7(Processor *cpu, const Instruction *inst)
{
    /**
     * xlat: AL¡ûDS:[(E)BX + unsigned AL]
     */
    if (inst->Main.Prefix.AddressSize) RET_NOT_IMPLEMENTED();

    u32 offset = cpu->EBX + cpu->AL;
	cpu->AL = cpu->MemRead8(offset);
    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()