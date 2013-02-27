#pragma once
 
#ifndef __ARIETIS_ARCHIVE_H__
#define __ARIETIS_ARCHIVE_H__
 
#include "Arietis.h"
#include "utilities.h"
#include "parallel.h"
#include "dbg/breakpoint.h"

struct Archive : public ISerializable, public MutexSyncObject {
    Archive();
    ~Archive();

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

    std::vector<Breakpoint>     Breakpoints;
    bool        IsTracerEnabled;
    bool        SkipDllEntries;
    bool        BreakOnCRTEntry;
    bool        IsTaintEnabled;
};
 
#endif // __ARIETIS_ARCHIVE_H__