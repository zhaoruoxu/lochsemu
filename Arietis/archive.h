#pragma once
 
#ifndef __ARIETIS_ARCHIVE_H__
#define __ARIETIS_ARCHIVE_H__
 
#include "Arietis.h"
#include "io.h"
#include "parallel.h"
#include "dbg/breakpoint.h"

struct Archive : public ISerializable, public MutexSyncObject {
    Archive();
    ~Archive();

    void        Serialize(Json::Value &root) const override;
    void        Deserialize(Json::Value &root) override;

    std::vector<Breakpoint>     Breakpoints;
};
 
#endif // __ARIETIS_ARCHIVE_H__