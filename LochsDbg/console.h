#pragma once

#ifndef __LOCHSDBG_CONSOLE_H__
#define __LOCHSDBG_CONSOLE_H__

#include "datastream.h"

class Console : public DataStream {
public:
    Console();
    virtual ~Console();

    virtual std::string  ReadLine();
private:
};

#endif // __LOCHSDBG_CONSOLE_H__
