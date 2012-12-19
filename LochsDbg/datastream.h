#pragma once

#ifndef __LOCHSDBG_DATASTREAM_H__
#define __LOCHSDBG_DATASTREAM_H__

#include "LochsDbg.h"

class DataStream {
public:
    DataStream() {}
    virtual ~DataStream() {}

    virtual std::string ReadLine() = 0;
};

#endif // __LOCHSDBG_DATASTREAM_H__
