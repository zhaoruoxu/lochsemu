#pragma once

#ifndef __CORE_STACK_H__
#define __CORE_STACK_H__

#include "lochsemu.h"
#include "section.h"

BEGIN_NAMESPACE_LOCHSEMU()

class LX_API Stack : public Section {
public:
    Stack(u32 base, u32 reserve, u32 commit, uint nModule);
    virtual ~Stack();

    LxResult    Update(Processor *cpu);
    u32         Top() const { return m_base + m_size; }
    u32         Bottom() const { return m_base; }
};


END_NAMESPACE_LOCHSEMU()

#endif // __CORE_STACK_H__
