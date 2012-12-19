#include "stdafx.h"
#include "stack.h"
#include "processor.h"

BEGIN_NAMESPACE_LOCHSEMU()


Stack::Stack( u32 base, u32 reserve, u32 commit, uint nModule )
: Section(SectionDesc("stack", nModule), base, reserve)
{
    Assert(PAGE_LOW(base) == 0);
    Assert(PAGE_LOW(reserve) == 0);
    Assert(PAGE_LOW(commit) == 0);

    V( Commit(base, reserve, PAGE_READWRITE) );
}

Stack::~Stack()
{

}

LochsEmu::LxResult Stack::Update( Processor *cpu )
{
    if (cpu->ESP > Top())
        return LX_RESULT_INVALID_ADDRESS;
    if (cpu->ESP <= Bottom())
        return LX_RESULT_NO_MEMORY;

    RET_SUCCESS();
}

END_NAMESPACE_LOCHSEMU()
