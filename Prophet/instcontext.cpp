#include "stdafx.h"
#include "instcontext.h"

const std::string InstContext::FlagNames[] = {
    "OF", "SF", "ZF", "AF", "PF", "CF", "DF",/* "TF", "IF", "DF", "NT", "RF" */
};

const std::string InstContext::RegNames[] = {
    "Eax", "Ecx", "Edx", "Ebx", "Esp", "Ebp", "Esi", "Edi", "Eip",
};
