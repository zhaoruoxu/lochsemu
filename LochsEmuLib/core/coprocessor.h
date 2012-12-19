#pragma once

#ifndef __CORE_COPROCESSOR_H__
#define __CORE_COPROCESSOR_H__

#include "lochsemu.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define LX_X87_REGS     8


typedef u8 FPUReg[10];

struct FPUStatusReg {
    u8  IE  : 1;    /*  0 Invalid Operation */
    u8  DE  : 1;    /*  1 Denormalized Operand */
    u8  ZE  : 1;    /*  2 Zero Divide */
    u8  OE  : 1;    /*  3 Overflow */
    u8  UE  : 1;    /*  4 Underflow */
    u8  PE  : 1;    /*  5 Precision */
    u8  SF  : 1;    /*  6 Stack Fault */
    u8  ES  : 1;    /*  7 Error Summary Status */
    u8  C0  : 1;    /*  8 Condition Code */
    u8  C1  : 1;    /*  9 Condition Code */
    u8  C2  : 1;    /* 10 Condition Code */
    u8  TOP : 3;    /* 11 Top of Stack Pointer */
    u8  C3  : 1;    /* 14 Condition Code */
    u8  Busy: 1;    /* 15 FPU Busy */
};

struct FPUControlReg {
    u8  IM  : 1;    /*  0 Invalid Operation */
    u8  DM  : 1;    /*  1 Denormal Operand */
    u8  ZM  : 1;    /*  2 Zero Divide */
    u8  OM  : 1;    /*  3 Overflow */
    u8  UM  : 1;    /*  4 Underflow */
    u8  PM  : 1;    /*  5 Precision */
    u8  REV0: 1;    /*  6 Reserved */
    u8  REV1: 1;    /*  7 Reserved */
    u8  PC  : 2;    /*  8 Precision Control */
    u8  RC  : 2;    /* 10 Rounding Control */
    u8  X   : 1;    /* 12 Infinity Control */
    u8  REV2: 3;    /* 13 Reserved */
};

struct FpuContext {
    u16     ControlWord;
    u16     Reserved0;
    u16     StatusWord;
    u16     Reserved1;
    u16     TagWord;
    u16     Reserved2;
    u32     InstructionPointer;
    u16     CodeSegment;
    u16     Reserved3;
    u32     OperandAddress;
    u16     DataSegment;
    u16     Reserved4;
    FPUReg  ST[8];
};

class LX_API Coprocessor {
public:
    Coprocessor();
    ~Coprocessor();
    
public:
    LxResult        Initialize      (void);
    void            Reset           (void);
    void            Wait            (void);
    void            RestoreContext  (void);
    void            SaveContext     (void);
    FpuContext*     Context         (void) { return &m_context; }

private:
    FpuContext      m_context;
    bool            m_preserveContext;
};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_COPROCESSOR_H__
